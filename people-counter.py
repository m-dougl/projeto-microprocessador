#Importa as bibliotecas relevantes
import numpy as np
import cv2
import requests
import socket
 
clientSocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM);
clientSocket.connect(("192.168.0.8",80)) #Conecta ao webserver da ESP32
 
cap = cv2.VideoCapture('http://192.168.0.7:81/stream') #URL para a conexão à transmissão da ESP32-CAM 
 
'''
    Função que administra a temperatura em função da quantidade
    de pessoas monitoradas pela esp32cam
'''
def temperature_management(i): 
    
   if 20<=i<=16:
       t = 18
       return t
 
   if 15<=i<=11:
       t = 19
       return t
 
   if 10<=i<=8:
       t = 20
       return t
 
   if 7<=i<=5:
       t = 21
       return t
 
   if 4<=i<=3:
       t = 22
       return t
 
   if 2<=i<=1:
       t = 23
       return t
 
   if i==0 :
       t = 'desligar'
       return t
 
while True:
   ret, frame=cap.read(0)
  
   #Aplicar o retângulo de reconhecimento de pessoas ao frame
   hog = cv2.HOGDescriptor()
   hog.setSVMDetector(cv2.HOGDescriptor_getDefaultPeopleDetector())#Padrão #pré treinado pela lib OpenCV para reconhecimento de pessoas 
   boxes, weights = hog.detectMultiScale(frame, winStride=(8,8) )
   boxes = np.array([[x, y, x + w, y + h] for (x, y, w, h) in boxes])
   i = 0
   for (xA, yA, xB, yB) in boxes:
     
       cv2.rectangle(frame, (xA, yA), (xB, yB),
                        (0, 255, 0), 2) 
       i+=1 #Incremeta 1 para cada retângulo (pessoa) 
       temp_desejada = temperature_management(i) #Armazena a #temperatura sugerida pela equipa
       clientSocket.send(temp_desejada) #Deveria enviar o valor ao webserver, mas a equipe não conseguiu fazer esta parte funcionar funcionar
       print(i) #Imprime o número de pessoas na tela           
 
   gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY) #Aplica um filtro de escala de cinza na imagem recebida pela ESP32-CAM
 
   cv2.imshow('Transmissao ao vivo',gray) #Inicia o monitoramento em #tempo real abrindo uma janela com a transmissão da ESP32-CAM
 
   if cv2.waitKey(1) & 0xFF == ord('q'): #Comando para finalizar a #conexao entre a esp32 cam e o OpenCV caso a tecla ‘q’ seja apertada
       break
 
cap.release()
cv2.destroyAllWindows #Comando para manter a janela do OpenCV acima de #todas as outras janela