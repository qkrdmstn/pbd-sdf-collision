## 📑 목차
- [🧾 프로젝트 개요](#-프로젝트-개요)
- [📆 개발 인원 및 기간](#-개발-인원-및-기간)
- [🛠️ 주요 기능](#️-주요-기능)
- [💻 사용 기술](#-사용-기술)
- [📷 결과 영상](#-결과-영상)
- [💡 프로젝트 회고](#-프로젝트-회고)
- [🔖 관련 블로그 글](#-관련-블로그-글)
- [📚 참고 자료](#-참고-자료)

<br>

## 📷 결과 영상
<p align="center">
  <img src="https://github.com/user-attachments/assets/cd435f89-39eb-44a6-a240-41c691e5e963" width="45%">
  <img src="https://github.com/user-attachments/assets/959a3b2f-9261-4467-856a-0d8e0f62855e" width="45%"><br>
  <img src="https://github.com/user-attachments/assets/4e1b82a9-a941-41d4-b9a8-480f4b562da4" width="45%">
  <img src="https://github.com/user-attachments/assets/72b28dcd-94e8-46ac-81ec-90e0ca17f708" width="45%">
</p>

[🔗 결과 영상 링크 (Youtube)](https://www.youtube.com/playlist?list=PLL7N-Nw3U-P1VskT4llhvH_EJs00NhZ-c)

<br>

## 🧾 프로젝트 개요
- 이 프로젝트는 3D 삼각형 메시로 구성된 오브젝트에 대해 **Signed Distance Field(SDF)** 를 생성합니다.
- **K-D 트리(K-dimensional tree)** 를 활용하여 각 복셀(Cell)의 최근접 삼각형과의 거리를 효율적으로 계산하는 것을 목표로 합니다.

<br>

## 📆 개발 인원 및 기간
- 1인 개발
- 2023년 6월 ~ 2024년 1월 (약 8개월)

<br>

## 🛠️ 주요 기능
- 입력 메시로부터 Signed Distance Field(SDF) 3D 격자 생성
- K-D 트리 기반 최단 거리 계산 최적화
- 삼각형-점 거리 계산 로직 구현
- SDF 시각화 및 디버깅 도구 포함
- 생성된 SDF를 활용한 충돌 감지 및 처리

<br>

## 💻 사용 기술
### 개발 언어

<img src="https://img.shields.io/badge/c-A8B9CC?style=for-the-badge&logo=c&logoColor=white"> <img src="https://img.shields.io/badge/c++-00599C?style=for-the-badge&logo=cplusplus&logoColor=white">

### 라이브러리
<img src="https://img.shields.io/badge/opengl-5586A4?style=for-the-badge&logo=opengl&logoColor=white"> <img src="https://img.shields.io/badge/opencv-5C3EE8?style=for-the-badge&logo=opencv&logoColor=white">

### 시뮬레이션 기법
- Position-Based Dynamics (PBD)
- Signed Distance Field (SDF)
- Collision Detection & Response
- Spatial Hashing
- K-D Tree 탐색 (최근점 삼각형 탐색)
- 점-삼각형 간 거리 계산
- AABB 기반 culling
  
<br>

## 💡 프로젝트 회고

- 기존에는 수학적으로 정의 가능한 물체에만 적용 가능한 SDF 기반 충돌 처리 기법을 사용했지만,  
이번 프로젝트에서는 **복잡한 임의의 메시 오브젝트에 대해서도 SDF를 생성할 수 있는 방식**을 구현했습니다.  

- 특히, **K-D 트리를 활용한 최근접 삼각형 탐색 최적화**를 통해, 모든 면과의 거리 계산 없이도  
정확한 충돌 감지를 실시간으로 수행할 수 있는 구조를 설계했습니다.  

- 이러한 최적화 경험은 **게임 클라이언트 개발에서 실시간 충돌 처리, 지형 분석, 물리 반응 시스템 등**에 직접적으로
  응용될 수 있으며, 게임의 퍼포먼스를 고려한 구조적인 문제 해결 능력을 키우는 데 큰 도움이 되었습니다.


<br>

## 🔖 관련 블로그 글
- [🔗 자세한 구현 및 학습 과정 정리 (Tistory)](https://coding-l7.tistory.com/category/%EB%AC%BC%EB%A6%AC%20%EA%B8%B0%EB%B0%98%20%EC%8B%9C%EB%AE%AC%EB%A0%88%EC%9D%B4%EC%85%98/Cloth%20Simulation)

<br>
  
## 📚 참고 자료

### Generating SDF
- [Generating Signed Distance Fields from Triangle Meshes – Bærentzen & Aanæs](https://www2.imm.dtu.dk/pubdb/edoc/imm1289.pdf)

### SDF:
- https://mshgrid.com/2021/02/07/computing-the-sign-of-discrete-distance-fields/

### AABB tree:
- https://mshgrid.com/2021/01/17/aabb-tree/

### K-D tree:
- https://www.baeldung.com/cs/k-d-trees#:~:text=A%20K%2DD%20Tree%20is%20a,one%20of%20the%20K%20dimensions.

