# Iot mini Project (반려묘 자동 급식 및 건강 관리)

### 👋🏻소개글

---

📌 프로젝트 명 : 반려견 홈케어 시스템 (Pet Health & Feed control System)

📝 프로젝트 개요 : 1인 가구의 증가와 함께 반려동물 시장 또한 증가하고 있다. 특히 반려 케어 시스템에 많은 관심과 중이 되고 있다. 1인 가구의 특성상 집을 비우는 시간이 많기 때문에 혼자 있는 반려동물을 걱정하는 사람이 많다. 이를 위해 반려인이 원격으로 영상을 관제하면서 먹이를 보급해주고, 맥박 측정 시스템을 이용함으로써 반려동물의 건강관리가 가능한 시스템을 개발하였다.

📆 프로젝트 기간 : 2023년 4월 1일 ~ 4월 10일

![1](https://user-images.githubusercontent.com/77330806/233028056-86eb1e7a-0556-4717-8175-39e449c91926.png)
### 🌐Youtube(최종 동영상)

---

[1️⃣ 최종 구현 동영상](https://www.youtube.com/watch?v=g_JCp-s1soQ)

[2️⃣ 최종 구현 동영상](https://www.youtube.com/watch?v=DWTKlE7tkXk)

[3️⃣ 최종 구현 동영상](https://www.youtube.com/watch?v=G1rCZroqQss)

[4️⃣ 최종 구현 동영상](https://www.youtube.com/watch?v=f-995GGhyLY)


## 👨‍👨‍👧‍👧프로젝트 팀원

---

| 이  름 | 담당 파트 |
| --- | --- |
| 김윤희 | STM-32(HC-SR04, DH11, OLED), Wifi통신, 보고서 작성, Notion 관리 |
| 양성모 | Arudino Main, Jetson-Raspberry Socket 통신, Android, Maria DB 설정, GitHub 관리 |



### 👤아이디어 구상

---

![2](https://user-images.githubusercontent.com/77330806/233028252-0ec85b4f-614d-4afc-b8bf-3a1510b16aec.jpg)


### 👁️아이디어 구현계획

---

1. 기능 1단계 : 안드로이드 앱구현을 통하여, Servo 모터 2개와 Camera 를 상,하,좌,우로 제어하며 실시간 모니터링 (앱의 방향키를 눌러 문자열을 전송하여  servo 모터 제어)
2. 기능 2단계 : 심박센서를 이용하여 반려묘의 심전도를 측정한 후 일정 기준치를 넘을시 이상이 감지됨을 블루투스을 이용하여 반려인에게 위치 및 반려묘의 상태알람이 울리는 서비스 (실시간으로 위치파악 가능)

1. 기능 3단계 : 타이머 기능으로 주기적으로 배급하거나 적외선 센서를 통하여 반려묘가 자동 배급기주변에 왔을때, 앱을통해 ‘움직임이 감지됨’  혹은 ‘먹이 배급을 시작합니다’ 등의 알림기능을 활성화, 동시에 servo 모터를 통하여 배급기가 작동됨

![3](https://user-images.githubusercontent.com/77330806/233029956-364e9dd3-0401-493a-892d-df8fec1fd55a.png)

### 📋개발 환경

---

- STM-32 → NUCLEO-F429ZI
- ARDUINO MEGA
- Raspberry Pi 4
- MariaDB
- Android
- Ubuntu 22.04

![4](https://user-images.githubusercontent.com/77330806/233030038-f4adc8f4-e578-4b4c-9198-d1d5e030d712.png)


### 📍구성도

---

![5](https://user-images.githubusercontent.com/77330806/233030090-9110a2c1-0d26-4f9a-a0d8-4743b7d73e04.png)

### ✨단계별 진행사항

---

## Step.01 - 각 기기별 테스트

- 라즈베리파이- csi Camera 연동을 통한 gstreamer 사용
- NUCLEO-F429ZI - wifi(ESP 8266) 통신 테스트
- Servo Motor - NUCLEO-F429ZI pwm 제어
    

## Step.02 - 각 기기별 테스트(2)

- 초음파 센서(HC-SR04) -  NUCLEO-F429ZI 연동 test
- RTC 내장 기능을 이용한 time data 받기
- OLED을 활용한 화면 출력
- 안드로이드 TCP/IP 테스트
- PHP + 데이터베이스로 안드로이드 연동
- 라즈베리파이 서보모터제어 pwm
- 라즈베리파이 블루투스 통신, TCP/IP 통신
- 아두이노 심박센서 및 블루투스

## Step.03 - 기기 연동 및 통신 테스트

- 안드로이드 사료급식기 제어 및 데이터베이스 연동
- 심박센서 데이터베이스 연동

## Step.04 - 보고서 작성


