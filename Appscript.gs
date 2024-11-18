var userCache = {}; // Global cache to store user data

// Caches all user data from the sheet into the global cache
function cacheUsers(userSheet) {
  var data = userSheet.getDataRange().getValues(); // Fetch all user data in one go
  for (var i = 1; i < data.length; i++) { // Skip the header row
    userCache[data[i][0]] = data[i][1]; // Store ID as key, Name as value
  }
}

// Get user name by userId; if cache is empty, populate it
function getUserNameById(userId, userSheet) {
  if (Object.keys(userCache).length === 0) { // Populate cache if empty
    cacheUsers(userSheet);
  }
  return userCache[userId] || "Unknown"; // Return cached name or "Unknown"
}

function doGet(e) {
  if (!e.parameter || !e.parameter.user) {
    return ContentService.createTextOutput("Invalid Request"); // Exit early if no parameters
  }

  var userId = e.parameter.user;
  var spreadsheetId = "1gx8fBcYaQlh6jYRhZ2U9hI8mCAPqphBxb9q9qjxbdig"; 
  var spreadsheet = SpreadsheetApp.openById(spreadsheetId); 
  var logSheet = spreadsheet.getSheetByName("LogSheet"); 
  var userSheet = spreadsheet.getSheetByName("UserSheet");

  // Cache the user data if not already done
  var userName = getUserNameById(userId, userSheet);
  var timestamp = new Date();

  // Extract date and time separately
  var date = Utilities.formatDate(timestamp, Session.getScriptTimeZone(), "yyyy-MM-dd");
  var time = Utilities.formatDate(timestamp, Session.getScriptTimeZone(), "HH:mm:ss");

  // Append the log data in separate columns for date and time
  logSheet.appendRow([userId, userName, date, time]);

  // Return a simple success message
  return ContentService.createTextOutput("OK");
}

// Archive logs older than 30 days
function archiveOldLogs() {
  var spreadsheetId = "1gx8fBcYaQlh6jYRhZ2U9hI8mCAPqphBxb9q9qjxbdig";
  var spreadsheet = SpreadsheetApp.openById(spreadsheetId);
  var logSheet = spreadsheet.getSheetByName("LogSheet");
  var archiveSheet = spreadsheet.getSheetByName("ArchiveSheet");

  var data = logSheet.getDataRange().getValues(); // Get all data in one batch
  var currentDate = new Date();
  var archiveThreshold = new Date();
  archiveThreshold.setDate(currentDate.getDate() - 30); // 30 days ago

  // Use batch operations to append and remove rows
  var rowsToArchive = [];

  for (var i = 1; i < data.length; i++) { // Start from the second row (skip header)
    var logDate = new Date(data[i][2] + " " + data[i][3]); // Combine date and time for comparison
    if (logDate < archiveThreshold) {
      rowsToArchive.push(data[i]); // Add to archive list
    }
  }

  // Archive and delete in bulk to minimize operations
  if (rowsToArchive.length > 0) {
    archiveSheet.getRange(archiveSheet.getLastRow() + 1, 1, rowsToArchive.length, rowsToArchive[0].length)
      .setValues(rowsToArchive); // Append to archive sheet
    for (var j = rowsToArchive.length - 1; j >= 0; j--) {
      logSheet.deleteRow(j + 2); // Delete from log sheet (offset by 1 due to header)
    }
  }
}

// Set up the trigger to run the archive function every 7 days
function createTimeDrivenTrigger() {
  ScriptApp.newTrigger('archiveOldLogs')
    .timeBased()
    .everyDays(7) // Run once a week
    .create();
}
