"""
import subprocess
subprocess.run(['python', 'segment\predict.py', '--view-img'])
 'segment\predict.py'
"""
import subprocess
subprocess.run(['python', 'segment\predict.py', '--source', '0','--weights','yolov5l.pt', '--view-img'])