# turtle bot

turtlebot3_ws/           <-- [루트] 워크스페이스 (공장)
├── build/               <-- 자동 생성됨 (건드리지 않음)
├── devel/               <-- 자동 생성됨 (실행 파일 등)
└── src/                 <-- [중요] 소스 코드는 여기만 들어감
    └── turtlebot        <-- [Git Root] turtlebot(제품)
        ├── .git/
        ├── README.md        
        ├── CMakeLists.txt
        ├── package.xml
        ├── src*         <-- QR인식, 이동하는 소스
        └── src/bin      <-- 실행파일

## 기능

- 저장된 맵을 불러 들여서 대기 장소에서 대기한다.
- 리스닝 포트를 개방, 특정 명령어가 아규먼트(위치 문자열)이 들어오면,
- 이미지(QR)를 읽기 위해서 카메라 모쥴을 오픈 QR을 코드를 스캔한다.
- 인식이 되면, QR에 적힌 위치를 내부에서 맵 로케이션으로 변경한 후 이 로게이션으로 이동한다.
- 이동 후 10초 대기 후 대기 장소로 돌아간다.

## 보호 및 추가 기능

- 라이다로 위험감지. 회피기동, 경광등 또는 음성경고 발생
- 네비게이션 중 길이 막히면 우회로 탐색 후 이동

## OS / Dev Tools

- Ubuntu-22.04
- C(98)/C++(11)
- Opencv v10.x

## Hardware

- Robotis turtlebot3(hamburger)
- camara3 module
- lidar sensor(ld-1)



