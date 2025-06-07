import sqlite3
import os

import sqlite3
import os

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
                    isWiFiConnected BOOLEAN
                )
            ''')
            conn.commit()
            conn.close()

    def log_status(self, data):
        conn = sqlite3.connect(self.db_path)
        c = conn.cursor()
        c.execute('''
            INSERT INTO status_log (
                isRunning, uuid, uptime, controllerStartMSec, temperatureSmoker,
                temperatureFood, temperatureTarget, fanPWM, doorPosition, RSSI,
                bars, ipAddress, isWiFiConnected
            ) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)
        ''', (
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
            data.get('isWiFiConnected')
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
        
    def get_status_since(self, start_timestamp):
        """Retrieve all status records since the given UNIX timestamp."""
        conn = sqlite3.connect(self.db_path)
        conn.row_factory = sqlite3.Row
        c = conn.cursor()
        c.execute("""
            SELECT * FROM status_log
            WHERE strftime('%s', timestamp) >= ?
            ORDER BY timestamp ASC
        """, (int(start_timestamp),))
        rows = c.fetchall()
        conn.close()
        return [dict(row) for row in rows]