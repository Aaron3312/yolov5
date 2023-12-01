"""
import subprocess
subprocess.run(['python', 'segment\predict.py', '--view-img'])
 'segment\predict.py'
"""
import subprocess
#subprocess.run(['python', 'detect.py', '--source', 'http://192.168.100.114:4747/video','--weights','yolov5l.pt', '--view-img'])
#subprocess.run(['python', 'detect.py', '--source', '0','--weights','yolov5l.pt', '--view-img'])
#subprocess.run(['python', 'funcionGPT.py'])
#subprocess.run(['python', 'detect.py', '--source', 'http://192.168.17.162:80/480x320.mjpeg','--weights','yolov5l.pt', '--view-img', '--max-det', '4'])
subprocess.run(['python', 'detect.py', '--source', '3','--weights','yolov5l.pt', '--view-img', '--max-det', '4'])
#subprocess.run(['python', 'detect.py', '--source', 'http://10.43.44.14:4747/video','--weights','yolov5l.pt', '--view-img', '--max-det', '4'])


