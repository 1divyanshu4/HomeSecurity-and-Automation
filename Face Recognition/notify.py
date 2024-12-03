from pushbullet import Pushbullet

# Your Pushbullet access token
ACCESS_TOKEN = "o.TVGr87sdN7lxs9AybJVNI8j0ZuCllb8s"

# Path to the image file you want to send
IMAGE_PATH = "/home/divyanshu/Project/Face_Recognition/Intruder/Test.jpg"

# Initialize the Pushbullet object
pb = Pushbullet(ACCESS_TOKEN)

try:
    # Open the image file in binary read mode
    with open(IMAGE_PATH, "rb") as pic:
        # Upload the file to Pushbullet
        file_data = pb.upload_file(pic, "picture.jpg")

    # Send the file to your devices
    push = pb.push_file(**file_data)

    print("Photo sent successfully!")
except Exception as e:
    print(f"Error: {e}")
