import sqlite3
import os
import datetime

class Database:
    def __init__(self, db_path='database.db'):
        self.db_path = db_path
        self._init_db()

    def _init_db(self):
        if not os.path.exists(self.db_path):
            conn = sqlite3.connect(self.db_path)
            c = conn.cursor()
            c.execute('''
                CREATE TABLE status_log (
                    id INTEGER PRIMARY KEY AUTOINCREMENT,
                    timestamp DATETIME DEFAULT CURRENT_TIMESTAMP,
                    isRunning BOOLEAN,
                    uuid TEXT,
                    uptime INTEGER,
                    controllerStartMSec INTEGER,
                    temperatureSmoker INTEGER,
                    temperatureFood INTEGER,
                    temperatureTarget INTEGER,
                    fanPWM INTEGER,
                    doorPosition INTEGER,
                    RSSI INTEGER,
                    bars INTEGER,
                    ipAddress TEXT,
                    isWiFiConnected BOOLEAN,
                    networkName TEXT,
                    temperatureError INTEGER
                )
            ''')
            conn.commit()
            conn.close()

    def log_status(self, data):
        conn = sqlite3.connect(self.db_path)
        c = conn.cursor()
        # Use local time in ISO format for the timestamp
        local_timestamp = datetime.datetime.now().strftime('%Y-%m-%d %H:%M:%S')
        c.execute('''
            INSERT INTO status_log (
                timestamp, isRunning, uuid, uptime, controllerStartMSec, temperatureSmoker,
                temperatureFood, temperatureTarget, fanPWM, doorPosition, RSSI,
                bars, ipAddress, isWiFiConnected, networkName, temperatureError
            ) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?,?)
        ''', (
            local_timestamp,
            data.get('isRunning'),
            data.get('uuid'),
            data.get('uptime'),
            data.get('controllerStartMSec'),
            data.get('temperatureSmoker'),
            data.get('temperatureFood'),
            data.get('temperatureTarget'),
            data.get('fanPWM'),
            data.get('doorPosition'),
            data.get('RSSI'),
            data.get('bars'),
            data.get('ipAddress'),
            data.get('isWiFiConnected'),
            data.get('networkName'),
            data.get('temperatureError') 
        ))
        conn.commit()
        conn.close()
        
    def cleanup_old_records(self):
        """Delete records older than 3 months."""
        conn = sqlite3.connect(self.db_path)
        c = conn.cursor()
        # SQLite's datetime is in UTC, so we use datetime('now', '-3 months')
        c.execute("""
            DELETE FROM status_log
            WHERE timestamp < datetime('now', '-3 months')
        """)
        conn.commit()
        conn.close()
        
    def get_status_since(self, run_start_signature):
        """
        Retrieve all status records from the database that correspond to the current running session,
        i.e., records with uptime >= run_start_signature['uptime'] and
        controllerStartMSec >= run_start_signature['controllerStartMSec'].
        """
        conn = sqlite3.connect(self.db_path)
        conn.row_factory = sqlite3.Row
        c = conn.cursor()
        c.execute("""
            SELECT * FROM status_log
            WHERE uptime >= ? AND controllerStartMSec = ?
            ORDER BY timestamp ASC
        """, (
            run_start_signature.get("uptime", 0),
            run_start_signature.get("controllerStartMSec", 0)
        ))
        rows = c.fetchall()
        conn.close()
        return [dict(row) for row in rows]