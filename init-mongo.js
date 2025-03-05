db = db.getSiblingDB("oyster_db");  // Switch to the database

// Create collections if they don't exist
if (!db.getCollectionNames().includes("farms")) {
    db.createCollection("farms");
}

if (!db.getCollectionNames().includes("sensor_active_data")) {
    db.createCollection("sensor_active_data");
}

if (!db.getCollectionNames().includes("sensor_archive_data")) {
    db.createCollection("sensor_archive_data");
}

if (!db.getCollectionNames().includes("system_active_levels")) {
    db.createCollection("system_active_levels");
}

if (!db.getCollectionNames().includes("system_archive_levels")) {
    db.createCollection("system_archive_levels");
}

if (!db.getCollectionNames().includes("lift_active_schedule")) {
    db.createCollection("lift_active_schedule");
}

if (!db.getCollectionNames().includes("lift_archive_schedule")) {
    db.createCollection("lift_archive_schedule");
}

print("Collections initialized in MongoDB");
