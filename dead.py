"""
import subprocess
subprocess.run(['python', 'segment\predict.py', '--view-img'])
 'segment\predict.py'
"""
import subprocess
#subprocess.run(['python', 'detect.py', '--source', 'http://192.168.100.114:4747/video','--weights','yolov5l.pt', '--view-img'])
subprocess.run(['python', 'detect.py', '--source', '0','--weights','yolov5l.pt', '--view-img'])
subprocess.run(['python', 'funcionGPT.py'])

