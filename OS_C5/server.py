#!/usr/bin/env python3
import http.server
import subprocess
import json
import os

PORT = 8080
SIMULATOR = "./simulator.exe"
class Handler(http.server.BaseHTTPRequestHandler):
    def do_GET(self):
        files = {
            "/":           ("index.html", "text/html"),
            "/index.html": ("index.html", "text/html"),
            "/style.css":  ("style.css",  "text/css"),
            "/app.js":     ("app.js",     "application/javascript"),
        }

        if self.path in files:
            filename, content_type = files[self.path]
            try:
                with open(filename, "rb") as f:
                    content = f.read()
                self.send_response(200)
                self.send_header("Content-Type", content_type)
                self.end_headers()
                self.wfile.write(content)
            except FileNotFoundError:
                self.send_error(404, f"File not found: {filename}")
        else:
            self.send_error(404, "Not found")
    def do_POST(self):
        if self.path != "/run":
            self.send_error(404, "Not found")
            return

        
        length    = int(self.headers.get("Content-Length", 0))
        body      = self.rfile.read(length).decode("utf-8")

        try:
           
            result = subprocess.run(
                [SIMULATOR],
                input=body,
                capture_output=True,
                text=True,
                timeout=10
            )

            output = result.stdout.strip()

            
            if not output:
                raise ValueError("Empty output from simulator")

            
            data = json.loads(output)

            
            self.send_response(200)
            self.send_header("Content-Type", "application/json")
            self.send_header("Access-Control-Allow-Origin", "*")
            self.end_headers()
            self.wfile.write(json.dumps(data).encode())

        except subprocess.TimeoutExpired:
            self._send_error("Simulator timed out")
        except json.JSONDecodeError as e:
            self._send_error(f"Invalid JSON from simulator: {str(e)}")
        except Exception as e:
            self._send_error(str(e))

   
    def _send_error(self, message):
        self.send_response(500)
        self.send_header("Content-Type", "application/json")
        self.end_headers()
        error = json.dumps({"error": message})
        self.wfile.write(error.encode())

    
    def log_message(self, format, *args):
        pass


if __name__ == "__main__":
    
    if not os.path.exists(SIMULATOR):
        print(f"❌ Error: '{SIMULATOR}' not found. Run 'make' first.")
        exit(1)

    print(f"✅ Server running at http://localhost:{PORT}")
    print(f"   Open your browser and go to: http://localhost:{PORT}")
    print(f"   Press Ctrl+C to stop.\n")

    server = http.server.HTTPServer(("", PORT), Handler)
    server.serve_forever()