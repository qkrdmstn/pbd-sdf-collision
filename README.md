# 📑 목차
- [🧾 프로젝트 개요](#-프로젝트-개요)
- [📆 개발 인원 및 기간](#-개발-인원-및-기간)
- [🛠️ 주요 기능](#️-주요-기능)
- [💻 사용 기술](#-사용-기술)
- [📷 시연 영상](#-시연-영상)
- [💡 프로젝트 회고](#-프로젝트-회고)
- [🔖 관련 블로그 글](#-관련-블로그-글)
- [📚 참고 자료](#-참고-자료)

# 🧾 프로젝트 개요
이 프로젝트는 3D 삼각형 메시로 구성된 고체에 대해 **Signed Distance Field(SDF)** 를 생성하고,  
 **K-D 트리(K-dimensional tree)**를 활용하여 각 복셀에 대한 최단 거리 계산을 최적화하는 것을 목표로 합니다.


# 📆 개발 인원 및 기간
- 1인 개발
- 2023년 6월 ~ 2024년 1월 (약 8개월)



# 🛠️ 주요 기능
- 입력 메시로부터 Signed Distance Field(SDF) 3D 격자 생성
- K-D 트리 기반 최단 거리 계산 최적화
- 삼각형-점 거리 계산 로직 구현
- SDF 시각화 및 디버깅 도구 포함
- 생성된 SDF를 활용한 충돌 감지 및 처리



# 💻 사용 기술
- **개발 언어**: C, C++
- **라이브러리**: OpenGL, OpenCV
- **시뮬레이션 기법**:
  - Position-Based Dynamics (PBD)
  - Signed Distance Field (SDF)
  - Collision Detection & Response
  - Spatial Hashing
  - K-D Tree 탐색 (최근점 삼각형 탐색)
  - 점-삼각형 간 거리 계산
  - AABB 기반 culling
  

# 📷 시연 영상
<p align="center">
  <img src="https://github.com/user-attachments/assets/cd435f89-39eb-44a6-a240-41c691e5e963" width="45%">
  <img src="https://github.com/user-attachments/assets/959a3b2f-9261-4467-856a-0d8e0f62855e" width="45%"><br>
  <img src="https://github.com/user-attachments/assets/4e1b82a9-a941-41d4-b9a8-480f4b562da4" width="45%">
  <img src="https://github.com/user-attachments/assets/72b28dcd-94e8-46ac-81ec-90e0ca17f708" width="45%">
</p>


[🔗 시연 영상 링크 (Youtube)](https://www.youtube.com/playlist?list=PLL7N-Nw3U-P1VskT4llhvH_EJs00NhZ-c)



# 💡 프로젝트 회고
이전에 진행했던 SDF 기반 충돌 처리는 방정식으로 표현 가능한 물체에 대해서만 가능했습니다.
이를 해결하기 위해 **임의의 메시에 대해 SDF**를 생성하고, 더 나아가 **K-D 트리 탐색을 통해 알고리즘적으로 연산을 줄이는 경험**을 할 수 있었습니다.

이러한 알고리즘 최적화 경험은 물리 기반 시뮬레이션, 게임 충돌 처리, 로봇 경로 탐색 등 다양한 실시간 시스템에 적용될 수 있습니다.  



# 🔖 관련 블로그 글
- [🔗 자세한 구현 및 학습 과정 정리 (Tistory)](https://coding-l7.tistory.com/category/%EB%AC%BC%EB%A6%AC%20%EA%B8%B0%EB%B0%98%20%EC%8B%9C%EB%AE%AC%EB%A0%88%EC%9D%B4%EC%85%98/Cloth%20Simulation)

  
# 📚 참고 자료

### Generating SDF
- [Generating Signed Distance Fields from Triangle Meshes – Bærentzen & Aanæs](https://www2.imm.dtu.dk/pubdb/edoc/imm1289.pdf)

### SDF:
- https://mshgrid.com/2021/02/07/computing-the-sign-of-discrete-distance-fields/

### AABB tree:
- https://mshgrid.com/2021/01/17/aabb-tree/

### K-D tree:
- https://www.baeldung.com/cs/k-d-trees#:~:text=A%20K%2DD%20Tree%20is%20a,one%20of%20the%20K%20dimensions.

