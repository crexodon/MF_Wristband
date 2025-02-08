from flask import Flask, request, jsonify
import sqlite3
from datetime import datetime
from flask_cors import CORS

app = Flask(__name__)
CORS(app)

def create_database():
    """Initialize the SQLite database with required tables"""
    conn = sqlite3.connect('rally.db')
    c = conn.cursor()
    
    c.execute('''CREATE TABLE IF NOT EXISTS participants (
                tag_id TEXT PRIMARY KEY,
                registered_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
                flag1 BOOLEAN DEFAULT FALSE,
                flag2 BOOLEAN DEFAULT FALSE,
                flag3 BOOLEAN DEFAULT FALSE,
                email TEXT,
                finish_time TIMESTAMP NULL
                )''')
    
    conn.commit()
    conn.close()

@app.route('/register', methods=['POST'])
def register_participant():
    """Endpoint for registering a new participant"""
    data = request.json
    if 'tag_id' not in data:
        return jsonify({'error': 'Missing required tag_id'}), 400
    
    conn = sqlite3.connect('rally.db')
    c = conn.cursor()
    
    try:
        c.execute('''INSERT INTO participants (tag_id, registered_at)
                    VALUES (?, ?)''',
                 (data['tag_id'], datetime.now().isoformat()))
        conn.commit()
        return jsonify({
            'status': 'success',
            'timestamp': datetime.now().isoformat()
        }), 201
    except sqlite3.IntegrityError:
        return jsonify({'error': 'Tag ID already exists'}), 400
    finally:
        conn.close()

@app.route('/flag/<int:tag_id>/<int:station>', methods=['POST'])
def collect_flag(tag_id, station):
    """Endpoint for collecting a flag at a station"""
    if station not in [1, 2, 3]:
        return jsonify({'error': 'Invalid station number'}), 400
    
    flag_column = f'flag{station}'
    conn = sqlite3.connect('rally.db')
    c = conn.cursor()
    
    try:
        c.execute(f'''UPDATE participants 
                     SET {flag_column}=TRUE, finish_time=?
                     WHERE tag_id=?''',
                 (datetime.now().isoformat(), tag_id))
        
        if c.rowcount == 0:
            return jsonify({'error': 'Participant not found'}), 404

        conn.commit()
        return jsonify({
            'status': 'success',
            'timestamp': datetime.now().isoformat()
        })
    finally:
        conn.close()

@app.route('/email/<int:tag_id>', methods=['POST'])
def submit_email(tag_id):
    """Endpoint for submitting email address"""
    data = request.json
    if 'email' not in data:
        return jsonify({'error': 'Missing required email'}), 400
    
    conn = sqlite3.connect('rally.db')
    c = conn.cursor()
    
    try:
        finishTime = datetime.now().isoformat()

        c.execute('''UPDATE participants 
                     SET email=?, finish_time=?
                     WHERE tag_id=?
                     RETURNING registered_at''',
                 (data['email'], finishTime, tag_id))

        participant = c.fetchone()

        if participant is None:
            return jsonify({'status': 'error', 'error': 'Participant not found'}), 404

        conn.commit()
        return jsonify({
            'status': 'success',
            'registered_at': participant[0],
            'finished_at': finishTime,
        })
    finally:
        conn.close()

@app.route('/stats', methods=['GET'])
def get_statistics():
    """Endpoint for getting rally statistics"""
    conn = sqlite3.connect('rally.db')
    c = conn.cursor()
    
    try:
        c.execute('''SELECT COUNT(*) as total_participants,
                           SUM(CASE WHEN flag1 THEN 1 ELSE 0 END) as flag1_count,
                           SUM(CASE WHEN flag2 THEN 1 ELSE 0 END) as flag2_count,
                           SUM(CASE WHEN flag3 THEN 1 ELSE 0 END) as flag3_count,
                           SUM(CASE WHEN email IS NOT NULL THEN 1 ELSE 0 END) as email_submissions
                    FROM participants''')
        
        stats = c.fetchone()
        return jsonify({
            'total_participants': stats[0],
            'flags_collected': {
                'flag1': stats[1],
                'flag2': stats[2],
                'flag3': stats[3]
            },
            'email_submissions': stats[4]
        })
    finally:
        conn.close()

if __name__ == '__main__':
    create_database()
    app.run(host='0.0.0.0', port=5000);