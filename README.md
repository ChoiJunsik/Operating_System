# 64비트 싱글코어 운영체제 개발 프로젝트

## 팀구성

- 최준식
- 우호진
- 유세빈
- 조세현

## 프로젝트의 목표

- 64비트 싱글코어 OS의 기능을 추가 구현한다.
1. 512B 부트로더를 두단계로 나누어 구현 (BIOS 인터럽트 서비스 이용)
2. 커널 부팅시 시스템초기화, 메모리 기능 구현( + BIOS를 통해 사용가능 메모리 알아내기, 페이지 테이블 이용 Memory Double Mapping )
3. Exception Handler 구현, Shell 구현
4. 스케줄러 구현
