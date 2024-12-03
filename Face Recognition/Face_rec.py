import cv2
import imutils
import pickle
import time
from datetime import datetime
from pathlib import Path
from imutils.video import VideoStream
from pushbullet import Pushbullet
import face_recognition
import threading

# Initialize necessary variables
currentname = "unknown"
last_detection_time = 0
detection_cooldown = 30  # Cooldown time in seconds before detecting the same unknown face again
encodingsP = "encodings.pickle"
cascade = "haarcascade_frontalface_default.xml"
ACCESS_TOKEN = "o.SA5FAH5SS1gFrXfKvlpBn24MCbtwF0Gm"

# Initialize Pushbullet object
pb = Pushbullet(ACCESS_TOKEN)

# Load known faces and embeddings along with OpenCV's Haar cascade
print("[INFO] Loading encodings and face detector...")
try:
    data = pickle.loads(open(encodingsP, "rb").read())
except FileNotFoundError:
    print(f"[ERROR] Encodings file '{encodingsP}' not found. Exiting...")
    exit(1)

detector = cv2.CascadeClassifier(cascade)

# Start the video stream
print("[INFO] Starting video stream...")
vs = VideoStream(src=0).start()
time.sleep(2.0)

# Function to create a folder with the current date and time
def create_output_folder():
    timestamp = datetime.now().strftime("%Y-%m-%d_%H-%M-%S")
    folder_path = Path(f"./IntruderLogs/{timestamp}")
    folder_path.mkdir(parents=True, exist_ok=True)
    return folder_path

# Function to send Pushbullet notifications
def send_pushbullet_notification(image_paths):
    try:
        # Send a text alert
        pb.push_note("Security Alert", "Unknown person detected! See the attached images.")

        # Attach all images
        for image_path in image_paths:
            with open(image_path, "rb") as pic:
                file_data = pb.upload_file(pic, image_path.name)
            pb.push_file(**file_data)

        print("[ALERT] Intruder images sent!")
    except Exception as e:
        print(f"[ERROR] Failed to send notification: {e}")

# Function to send Pushbullet notifications asynchronously
def send_pushbullet_notification_async(image_paths):
    notification_thread = threading.Thread(target=send_pushbullet_notification, args=(image_paths,))
    notification_thread.start()

# Function to record a 5-second video asynchronously
def record_video(output_folder, frame_width, frame_height):
    print("[INFO] Starting 5-second video recording...")
    video_path = output_folder / "recording.avi"
    out = cv2.VideoWriter(
        str(video_path),
        cv2.VideoWriter_fourcc(*'MJPG'),
        20.0,
        (frame_width, frame_height)
    )

    start_time = time.time()
    while time.time() - start_time < 5:  # Record for 5 seconds
        frame = vs.read()

        # Safety check for NoneType frame
        if frame is None:
            print("[ERROR] Could not capture frame during video recording.")
            break

        frame = imutils.resize(frame, width=frame_width)
        out.write(frame)

    out.release()
    print(f"[INFO] Video saved to {video_path}")

# Main loop variables
frame_counter = 0
FRAME_SKIP = 10  # Number of frames to skip (process every 10th frame)

# Main loop
while True:
    frame = vs.read()

    # Safety check for NoneType frame
    if frame is None:
        print("[ERROR] Could not capture frame from video stream. Exiting...")
        break

    # Increment frame counter
    frame_counter += 1

    # Process only every 10th frame
    if frame_counter % FRAME_SKIP != 0:
        continue

    # Reduce frame resolution for lower memory and CPU usage
    frame = imutils.resize(frame, width=320)
    gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
    rgb = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)

    rects = detector.detectMultiScale(gray, scaleFactor=1.1, minNeighbors=5, minSize=(30, 30), flags=cv2.CASCADE_SCALE_IMAGE)
    boxes = [(y, x + w, y + h, x) for (x, y, w, h) in rects]

    encodings = face_recognition.face_encodings(rgb, boxes)
    names = []

    for encoding in encodings:
        matches = face_recognition.compare_faces(data["encodings"], encoding)
        name = "Unknown"

        if True in matches:
            matchedIdxs = [i for (i, b) in enumerate(matches) if b]
            counts = {}
            for i in matchedIdxs:
                name = data["names"][i]
                counts[name] = counts.get(name, 0) + 1
            name = max(counts, key=counts.get)

        # Handle unknown face detection
        if name == "Unknown" and (time.time() - last_detection_time > detection_cooldown):
            last_detection_time = time.time()
            print("[ALERT] Unknown person detected!")

            # Create a new folder for this detection
            output_folder = create_output_folder()
            print(f"[INFO] Created folder: {output_folder}")

            # Capture 3 images at 1-second intervals
            captured_images = []
            for i in range(3):
                img_path = output_folder / f"captured_image_{i + 1}.jpg"
                cv2.imwrite(str(img_path), frame)
                print(f"[INFO] Image saved to {img_path}")
                captured_images.append(img_path)
                time.sleep(1)  # Wait 1 second before the next capture

            # Send Pushbullet notification asynchronously
            send_pushbullet_notification_async(captured_images)

            # Start the video recording in a separate thread
            frame_width = frame.shape[1]
            frame_height = frame.shape[0]
            video_thread = threading.Thread(target=record_video, args=(output_folder, frame_width, frame_height))
            video_thread.start()

        names.append(name)

    # Draw bounding boxes and names
    for ((top, right, bottom, left), name) in zip(boxes, names):
        cv2.rectangle(frame, (left, top), (right, bottom), (0, 255, 0), 2)
        y = top - 15 if top - 15 > 15 else top + 15
        cv2.putText(frame, name, (left, y), cv2.FONT_HERSHEY_SIMPLEX, 0.8, (255, 0, 0), 2)

    # Display the frame
    cv2.imshow("Facial Recognition is Running", frame)
    key = cv2.waitKey(1) & 0xFF

    if key == ord("q"):
        break

# Cleanup
print("[INFO] Cleaning up...")
cv2.destroyAllWindows()
vs.stop()
