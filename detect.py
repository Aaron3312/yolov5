# YOLOv5 🚀 by Ultralytics, AGPL-3.0 license
"""
Run YOLOv5 detection inference on images, videos, directories, globs, YouTube, webcam, streams, etc.

Usage - sources:
    $ python detect.py --weights yolov5s.pt --source 0                               # webcam
                                                     img.jpg                         # image
                                                     vid.mp4                         # video
                                                     screen                          # screenshot
                                                     path/                           # directory
                                                     list.txt                        # list of images
                                                     list.streams                    # list of streams
                                                     'path/*.jpg'                    # glob
                                                     'https://youtu.be/LNwODJXcvt4'  # YouTube
                                                     'rtsp://example.com/media.mp4'  # RTSP, RTMP, HTTP stream

Usage - formats:
    $ python detect.py --weights yolov5s.pt                 # PyTorch
                                 yolov5s.torchscript        # TorchScript
                                 yolov5s.onnx               # ONNX Runtime or OpenCV DNN with --dnn
                                 yolov5s_openvino_model     # OpenVINO
                                 yolov5s.engine             # TensorRT
                                 yolov5s.mlmodel            # CoreML (macOS-only)
                                 yolov5s_saved_model        # TensorFlow SavedModel
                                 yolov5s.pb                 # TensorFlow GraphDef
                                 yolov5s.tflite             # TensorFlow Lite
                                 yolov5s_edgetpu.tflite     # TensorFlow Edge TPU
                                 yolov5s_paddle_model       # PaddlePaddle
"""

import argparse
import csv
import os
import platform
import sys
from pathlib import Path
from tkinter import *
import socket
import datetime
import time
from time import sleep






import torch


import firebase_admin 
from firebase_admin import db, credentials

FILE = Path(__file__).resolve()
ROOT = FILE.parents[0]  # YOLOv5 root directory
if str(ROOT) not in sys.path:
    sys.path.append(str(ROOT))  # add ROOT to PATH
ROOT = Path(os.path.relpath(ROOT, Path.cwd()))  # relative

from ultralytics.utils.plotting import Annotator, colors, save_one_box

from models.common import DetectMultiBackend
from utils.dataloaders import IMG_FORMATS, VID_FORMATS, LoadImages, LoadScreenshots, LoadStreams
from utils.general import (LOGGER, Profile, check_file, check_img_size, check_imshow, check_requirements, colorstr, cv2,
                           increment_path, non_max_suppression, print_args, scale_boxes, strip_optimizer, xyxy2xywh)
from utils.torch_utils import select_device, smart_inference_mode


@smart_inference_mode()



def areaYporcentaje(sd):
    # Cálculo del área para cada cuadro delimitador
    area = (sd[2] - sd[0]) * (sd[3] - sd[1])
    area_minima = 0
    area_total = 265154
    porcentaje = (area - area_minima) / (area_total - area_minima) * 100
    #convert porcentaje to int 
    porcentaje = int(porcentaje)
    return porcentaje



def run(
        weights=ROOT / 'yolov5s.pt',  # model path or triton URL
        source=ROOT / 'data/images',  # file/dir/URL/glob/screen/0(webcam)
        data=ROOT / 'data/coco128.yaml',  # dataset.yaml path
        imgsz=(640, 640),  # inference size (height, width)
        conf_thres=0.25,  # confidence threshold
        iou_thres=0.45,  # NMS IOU threshold
        max_det=3,  # maximum detections per image
        device='',  # cuda device, i.e. 0 or 0,1,2,3 or cpu
        view_img=False,  # show results
        save_txt=False,  # save results to *.txt
        save_csv=False,  # save results in CSV format
        save_conf=False,  # save confidences in --save-txt labels
        save_crop=False,  # save cropped prediction boxes
        nosave=False,  # do not save images/videos
        classes=None,  # filter by class: --class 0, or --class 0 2 3
        agnostic_nms=False,  # class-agnostic NMS
        augment=False,  # augmented inference
        visualize=False,  # visualize features
        update=False,  # update all models
        project=ROOT / 'runs/detect',  # save results to project/name
        name='exp',  # save results to project/name
        exist_ok=False,  # existing project/name ok, do not increment
        line_thickness=3,  # bounding box thickness (pixels)
        hide_labels=False,  # hide labels
        hide_conf=False,  # hide confidences
        half=False,  # use FP16 half-precision inference
        dnn=False,  # use OpenCV DNN for ONNX inference
        vid_stride=1,  # video frame-rate stride
):





    
    #cosas del movimiento de carrito
    ESP_IP = "192.168.95.20"
    ESP_PORT = 8266
    
    mov = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    def vueltas(d11):
        if d11 == [0]:
            sleep(0.2)
            mov.send('f'.encode())
            
            d11 = [1]
        return d11

    def movimiento_derecha(d11):
        if d11 == [0]:
            sleep(0.2)
            mov.send('f'.encode())
            
            d11 = [1]

        return d11

    def movimiento_izquierda(a11):
        if a11 == 0:
            sleep(0.2)
            mov.send('g'.encode())
            
            a11 = 1
        return a11
    def movimiento_adelante(w11):
        if w11 == 0:
            for i in range(0, 10):
                mov.send('w'.encode())         
            w11 = 1
        return 
    def movimiento_stop(q11):
        if q11 == 0:
            mov.send('q'.encode())
            q11 = 1
        return q11

    #finaliza cosas del movimiento de carrito

    # Initialize Firebase
    cred = credentials.Certificate("credentials.json")
    firebase_admin.initialize_app(cred, {"databaseURL": "https://iot-robotarm-default-rtdb.firebaseio.com/"})
    ref = db.reference('/IAROB')
    #ref.delete()

    print("Se eliminaron todos los documentos en la colección especificada.")
    # end firebase

    # Initialize Car Code
    """
    ESP_IP = "192.168.168.20"
    ESP_PORT = 8266

    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    def key_pressed(c):
        print("Tecla presionada: ", c.char)
        s.send(c.char.encode())
    def key_released(c):
        print("Tecla liberada: ", c.char)
        s.send(c.char.encode())
    root = Tk()
    root.title("Control de Carro")

    frame = Frame(root)
    lbl_title = Label(frame, text="Control de Carro ROBOT")
    lbl_title.grid(row=0, column=0, pady=10, padx=10)

    frame.bind ("<KeyPress>", key_pressed)
    frame.bind ("<KeyRelease>", key_released)

    s.connect((ESP_IP, ESP_PORT))

    frame.pack()
    frame.focus_set()

    root.mainloop()
    """
    # end car code





    source = str(source)
    save_img = not nosave and not source.endswith('.txt')  # save inference images
    is_file = Path(source).suffix[1:] in (IMG_FORMATS + VID_FORMATS)
    is_url = source.lower().startswith(('rtsp://', 'rtmp://', 'http://', 'https://'))
    webcam = source.isnumeric() or source.endswith('.streams') or (is_url and not is_file)
    screenshot = source.lower().startswith('screen')
    desiteracion = 0
    objets = []
    porcentajed3 = []
    objetos_detectados = []
    cordenados = []
    tiempod = []
    po = 0
    c= 0


    if is_url and is_file:
        source = check_file(source)  # download

    center_point = round((10+20)/2), round((10+20)/2)
    # Directories
    save_dir = increment_path(Path(project) / name, exist_ok=exist_ok)  # increment run
    (save_dir / 'labels' if save_txt else save_dir).mkdir(parents=True, exist_ok=True)  # make dir

    # Load model
    device = select_device(device)
    model = DetectMultiBackend(weights, device=device, dnn=dnn, data=data, fp16=half)
    stride, names, pt = model.stride, model.names, model.pt
    imgsz = check_img_size(imgsz, s=stride)  # check image size
    determinalo = 0
    a11 = 0
    d11 = [0]
    w11 = 0
    q11 = 0

    #cosas pa mandarle al otro wn
    
    mov = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    mov.connect((ESP_IP, ESP_PORT))



    # Dataloader
    bs = 1  # batch_size
    if webcam:
        view_img = check_imshow(warn=True)
        dataset = LoadStreams(source, img_size=imgsz, stride=stride, auto=pt, vid_stride=vid_stride)
        bs = len(dataset)
    elif screenshot:
        dataset = LoadScreenshots(source, img_size=imgsz, stride=stride, auto=pt)
    else:
        dataset = LoadImages(source, img_size=imgsz, stride=stride, auto=pt, vid_stride=vid_stride)
    vid_path, vid_writer = [None] * bs, [None] * bs


    # Run inference
    model.warmup(imgsz=(1 if pt or model.triton else bs, 3, *imgsz))  # warmup
    seen, windows, dt = 0, [], (Profile(), Profile(), Profile())
    for path, im, im0s, vid_cap, s in dataset:
        determinalo += 1
        with dt[0]:
            im = torch.from_numpy(im).to(model.device)
            im = im.half() if model.fp16 else im.float()  # uint8 to fp16/32
            im /= 255  # 0 - 255 to 0.0 - 1.0
            if len(im.shape) == 3:
                im = im[None]  # expand for batch dim

        # Inference
        with dt[1]:
            visualize = increment_path(save_dir / Path(path).stem, mkdir=True) if visualize else False
            pred = model(im, augment=augment, visualize=visualize)

        # NMS
        with dt[2]:
            pred = non_max_suppression(pred, conf_thres, iou_thres, classes, agnostic_nms, max_det=max_det)

        # Second-stage classifier (optional)
        # pred = utils.general.apply_classifier(pred, classifier_model, im, im0s)

        # Define the path for the CSV file
        csv_path = save_dir / 'predictions.csv'

        # Create or append to the CSV file
        def write_to_csv(image_name, prediction, confidence):
            data = {'Image Name': image_name, 'Prediction': prediction, 'Confidence': confidence}
            with open(csv_path, mode='a', newline='') as f:
                writer = csv.DictWriter(f, fieldnames=data.keys())
                if not csv_path.is_file():
                    writer.writeheader()
                writer.writerow(data)

        # Process predictions
        for i, det in enumerate(pred):  # per image
            seen += 1
            if webcam:  # batch_size >= 1
                p, im0, frame = path[i], im0s[i].copy(), dataset.count
                s += f'{i}: '
            else:
                p, im0, frame = path, im0s.copy(), getattr(dataset, 'frame', 0)

            p = Path(p)  # to Path
            save_path = str(save_dir / p.name)  # im.jpg
            txt_path = str(save_dir / 'labels' / p.stem) + ('' if dataset.mode == 'image' else f'_{frame}')  # im.txt
            s += '%gx%g ' % im.shape[2:]  # print string
            gn = torch.tensor(im0.shape)[[1, 0, 1, 0]]  # normalization gain whwh
            imc = im0.copy() if save_crop else im0  # for save_crop
            annotator = Annotator(im0, line_width=line_thickness, example=str(names))
            if len(det):
                # Rescale boxes from img_size to im0 size
                det[:, :4] = scale_boxes(im.shape[2:], det[:, :4], im0.shape).round()
                      # Rescale boxes from img_size to im0 size
                      
                for *xyxy, conf, cls in reversed(det):
                    c1, c2 = (int(xyxy[0]), int(xyxy[1])), (int(xyxy[2]), int(xyxy[3]))
                    center_point = round((c1[0]+c2[0])/2), round((c1[1]+c2[1])/2)
                    circle = cv2.circle(im0,center_point,5,(0,255,0),2)
                    text_coord = cv2.putText(im0,str(center_point),center_point,cv2.FONT_HERSHEY_PLAIN,2,(0,0,255))



                # Print results
                for c in det[:, 5].unique():
                    n = (det[:, 5] == c).sum()  # detections per class
                
                
                
                                
            
                # Write results
                for *xyxy, conf, cls in reversed(det):
                    c = int(cls)  # integer class
                    label = names[c] if hide_conf else f'{names[c]}'
                    confidence = float(conf)
                    confidence_str = f'{confidence:.2f}'
                    sd = (int (xyxy[0]),int(xyxy[1]),int(xyxy[2]),(int(xyxy[3])))
                    sds = areaYporcentaje(sd) 
                    s += f"{n} {names[int(c)]} {sds} %{'s' * (n > 1)}, "  # add to 
                    #objetos detectados unicamente tiene que tener los objetos detectados en todo el video
                    nuevo_string = names[int(c)]
                    nuevo_cordenado = center_point
                    timestamp = datetime.datetime.now()
                    # convertimos el timestamp a formato hora minuto segundo
                    timestamp = timestamp.strftime("%H:%M:%S")
                    timestamp = str (timestamp)



                    if nuevo_string not in objetos_detectados:
                        objetos_detectados.append(nuevo_string)
                        cordenados.append(nuevo_cordenado)
                        porcentajed3.append(sds)
                        tiempod.append(timestamp)                        
                        print("Se detecto un nuevo objeto")
                    # Añadir el nuevo string al vector si no existe
                        
                    if nuevo_string in objetos_detectados:
                        # actualizar el valor de porcentaje de cercania y cordenadas
                        # buscar el indice del objeto en el vector
                        indice = objetos_detectados.index(nuevo_string)
                        # actualizar el valor de porcentaje de cercania
                        porcentajed3[indice] = sds
                        # actualizar el valor de cordenadas
                        cordenados[indice] = nuevo_cordenado
                        # actualizar el valor de tiempo
                        tiempod[indice] = timestamp

                        # actualizar el valor de objetos detectados
                        #objetos_detectados[indice] = nuevo_string
                        #print(objetos_detectados)
                        # actualizar el valor de tiempo




                    if determinalo % 100 == 0:
                        ref.update({'objetos detectados': objetos_detectados})
                        ref.update({'cordenados': cordenados})
                        ref.update({'porcentaje_de_cercania': porcentajed3})
                        ref.update({'tiempo': tiempod})
 # Coordenadas del centro de la pantalla
                x_centro = 320
                x_objetivo = center_point[0]
                rango_error = 40

                if names[int(c)] == "person":

                    if 1 < sds < 80:
                        # Determinar la dirección en la que el carro debe moverse
                        if x_centro - rango_error <= x_objetivo <= x_centro + rango_error:
                        #if rango_error == 40:
                            print("camine hacia adelante")
                            s += f"camine hacia adelante "
                            movimiento_adelante(w11)

                        
                        elif x_objetivo > x_centro + rango_error:
                            s += f"camine hacia la derecha: "
                            movimiento_derecha(d11)
                        elif x_objetivo < x_centro - rango_error:
                            s += f"camine hacia la izquierda: "
                            movimiento_izquierda(a11)
                        
                        else:
                            s += f"da vueltas como imbezil: "
                            vueltas(d11)
                        
                    elif sds > 80:
                        s += f"detente: "
                        movimiento_stop(q11)

                else:
                    s += f"da vueltas como imbezil: "
                    vueltas(d11)
                    #prueba
                    # Determinar la dirección en la que el carro debe moverse

                    # firebase data

                    #guardamos los datos de names sds y lo que tenga la s 


     #           else :
      #              if guardo[0][0] < 320:
##                       s += f"camine hacia la izquierda: "
  #                  if guardo[0][0] > 320:
   #                     s += f"camine hacia la derecha: "
    #                else:
     #                   print(guardo[0][0])
      #                  s += f"da vueltas como imbezil: " 

                    if save_csv:
                        write_to_csv(p.name, label, confidence_str)

                    if save_txt:  # Write to file
                        xywh = (xyxy2xywh(torch.tensor(xyxy).view(1, 4)) / gn).view(-1).tolist()  # normalized xywh
                        line = (cls, *xywh, conf) if save_conf else (cls, *xywh)  # label format
                        with open(f'{txt_path}.txt', 'a') as f:
                            f.write(('%g ' * len(line)).rstrip() % line + '\n')

                    if save_img or save_crop or view_img:  # Add bbox to image
                        c = int(cls)  # integer class
                        label = None if hide_labels else (names[c] if hide_conf else f'{names[c]} {conf:.2f}')
                        annotator.box_label(xyxy, label, color=colors(c, True))
                    if save_crop:
                        save_one_box(xyxy, imc, file=save_dir / 'crops' / names[c] / f'{p.stem}.jpg', BGR=True)
                

            # Stream results
            im0 = annotator.result()
            if view_img:
                if platform.system() == 'Linux' and p not in windows:
                    windows.append(p)
                    cv2.namedWindow(str(p), cv2.WINDOW_NORMAL | cv2.WINDOW_KEEPRATIO)  # allow window resize (Linux)
                    cv2.resizeWindow(str(p), im0.shape[1], im0.shape[0])
                cv2.imshow(str(p), im0)
                cv2.waitKey(1)  # 1 millisecond

            # Save results (image with detections)
            if save_img:
                if dataset.mode == 'image':
                    cv2.imwrite(save_path, im0)
                else:  # 'video' or 'stream'
                    if vid_path[i] != save_path:  # new video
                        vid_path[i] = save_path
                        if isinstance(vid_writer[i], cv2.VideoWriter):
                            vid_writer[i].release()  # release previous video writer
                        if vid_cap:  # video
                            fps = vid_cap.get(cv2.CAP_PROP_FPS)
                            w = int(vid_cap.get(cv2.CAP_PROP_FRAME_WIDTH))
                            h = int(vid_cap.get(cv2.CAP_PROP_FRAME_HEIGHT))
                        else:  # stream
                            fps, w, h = 30, im0.shape[1], im0.shape[0]
                        save_path = str(Path(save_path).with_suffix('.mp4'))  # force *.mp4 suffix on results videos
                        vid_writer[i] = cv2.VideoWriter(save_path, cv2.VideoWriter_fourcc(*'mp4v'), fps, (w, h))
                    vid_writer[i].write(im0)

                            
           


        # lo domrmimos unos segundos
        # Print time (inference-only)
        LOGGER.info(f"{s}{'' if len(det) else f'(no detections), Empieza a dar vueltas como imbezil,  {vueltas(d11)}' }{dt[1].dt * 1E3:.1f}ms")



        

    # Print results
    t = tuple(x.t / seen * 1E3 for x in dt)  # speeds per image
    LOGGER.info(f'Speed: %.1fms pre-process, %.1fms inference, %.1fms NMS per image at shape {(1, 3, *imgsz)}' % t)
    if save_txt or save_img:
        s = f"\n{len(list(save_dir.glob('labels/*.txt')))} labels saved to {save_dir / 'labels'}" if save_txt else ''
        LOGGER.info(f"Results saved to {colorstr('bold', save_dir)}{s}")
    if update:
        strip_optimizer(weights[0])  # update model (to fix SourceChangeWarning)


def parse_opt():
    parser = argparse.ArgumentParser()
    parser.add_argument('--weights', nargs='+', type=str, default=ROOT / 'yolov5s.pt', help='model path or triton URL')
    parser.add_argument('--source', type=str, default=ROOT / 'data/images', help='file/dir/URL/glob/screen/0(webcam)')
    parser.add_argument('--data', type=str, default=ROOT / 'data/coco128.yaml', help='(optional) dataset.yaml path')
    parser.add_argument('--imgsz', '--img', '--img-size', nargs='+', type=int, default=[640], help='inference size h,w')
    parser.add_argument('--conf-thres', type=float, default=0.25, help='confidence threshold')
    parser.add_argument('--iou-thres', type=float, default=0.45, help='NMS IoU threshold')
    parser.add_argument('--max-det', type=int, default=1000, help='maximum detections per image')
    parser.add_argument('--device', default='', help='cuda device, i.e. 0 or 0,1,2,3 or cpu')
    parser.add_argument('--view-img', action='store_true', help='show results')
    parser.add_argument('--save-txt', action='store_true', help='save results to *.txt')
    parser.add_argument('--save-csv', action='store_true', help='save results in CSV format')
    parser.add_argument('--save-conf', action='store_true', help='save confidences in --save-txt labels')
    parser.add_argument('--save-crop', action='store_true', help='save cropped prediction boxes')
    parser.add_argument('--nosave', action='store_true', help='do not save images/videos')
    parser.add_argument('--classes', nargs='+', type=int, help='filter by class: --classes 0, or --classes 0 2 3')
    parser.add_argument('--agnostic-nms', action='store_true', help='class-agnostic NMS')
    parser.add_argument('--augment', action='store_true', help='augmented inference')
    parser.add_argument('--visualize', action='store_true', help='visualize features')
    parser.add_argument('--update', action='store_true', help='update all models')
    parser.add_argument('--project', default=ROOT / 'runs/detect', help='save results to project/name')
    parser.add_argument('--name', default='exp', help='save results to project/name')
    parser.add_argument('--exist-ok', action='store_true', help='existing project/name ok, do not increment')
    parser.add_argument('--line-thickness', default=3, type=int, help='bounding box thickness (pixels)')
    parser.add_argument('--hide-labels', default=False, action='store_true', help='hide labels')
    parser.add_argument('--hide-conf', default=False, action='store_true', help='hide confidences')
    parser.add_argument('--half', action='store_true', help='use FP16 half-precision inference')
    parser.add_argument('--dnn', action='store_true', help='use OpenCV DNN for ONNX inference')
    parser.add_argument('--vid-stride', type=int, default=1, help='video frame-rate stride')
    
    opt = parser.parse_args()
    opt.imgsz *= 2 if len(opt.imgsz) == 1 else 1  # expand
    print_args(vars(opt))
    return opt


def main(opt):
    check_requirements(ROOT / 'requirements.txt', exclude=('tensorboard', 'thop'))
    run(**vars(opt))


if __name__ == '__main__':
    opt = parse_opt()
    main(opt)
