from flask import Flask, render_template, request
import subprocess
import os

app = Flask(__name__)

@app.route('/')
def home():
    return render_template('index.html')

@app.route('/summarize', methods=['POST'])
def summarize():
    user_input = request.form['text']

    # Save input to a temp file
    with open('input.txt', 'w') as f:
        f.write(user_input)

    # Run the C executable with redirected input
    result = subprocess.run(
        [".\summarizer.exe"],
        input=user_input.encode(),
        capture_output=True
    )

    summary_output = result.stdout.decode()

    return render_template('index.html', summary=summary_output)

if __name__ == '__main__':
    port = int(os.environ.get('PORT', 5000))
    app.run(host='0.0.0.0', port=port)
