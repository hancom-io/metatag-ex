# MetatagEX



## Description
>1. hwpx-owpml-model프로젝트를 활용한 예제 프로젝트이다.
>2. 메타태그는 한글 문서 자체나 표, 개체들에 지정될수 있는데 해당 프로젝트는 hwpx 문서안에 포함된 메타태그를 추출하여 콘솔(터미널)에 보여주거나 json 파일로 저장할 수 있으며, json파일을 입력받아 메타태그를 분류하여 콘솔(터미널)에 보여주거나 json파일로 저장할 수 있다.

## Usage
### Windows
- ex) MetatagEX.exe sl ^.*.hwpx
- 설명 : sl(sourcelist)를 이용하여 목록을 추출하고  ^.*.hwpx 정규식에 해당하는 파일을 파싱한다.

### Linux(Ubuntu)
- ex) ./MetatagEX dl f ^.*.hwpx input.json result.json
- 설명 : dl(destlist)를 이용하여 파일을 분류하고 입력으로 input.json을 사용한다. f(file) 옵션을 통해 result.json 파일에 결과를 저장한다.

* 옵션들
- sl(sourcelist) : 목록을 추출
- dl(destlist) : 입력받은 json 파일을 이용해서 파일을 분류
- f(file) : 파일로 결과를 저장
- dsc(descend) : 내림차순으로 결과를 출력 또는 저장 default는 오름차순
- pr(progress) : 진행 Percent를 콘솔(터미널)에 출력한다. 쓰레드로 동작한다.
- doc(document) : 한글 문서 자체에 부여된 메타태그에 대해서만 수행한다.
- help(help) : 도움말 출력

## Install
- Windows : buildscript.bat 파일 실행 또는 Visual Studio를 이용하여 CMake 열기(CMakeSettings.json 설정되어있음)후 직접 빌드
- Linux(Ubuntu) : buildscript.sh 실행 또는 직접빌드
- Windows 라이브러리 경로 : out/install/x64-Debug (CMakeSettings.json 의 installRoot에 설정되어 있음)
- buildscript.bat 파일 빌드시 컴파일러는 Visual Studio 15 2017 로 설정되어 있음
- 윈도우에서 빌드하기위해서는 CMake가 설치되어 있어야함.

## Build


## License


## Opensources
- rapidjson(MIT)

## Contribution
- 소스코드 indent는 공백으로 삽입
 
#### commit convention
- 제목(1 행)과 본문 사이 한 줄 띄워 분리
- 제목은 영문자 50자 이내, 작업 영역(도메인)과 작업 내용 요약 (예: "engine: xlsx 렌더링 속도 개선")
- 제목은 한글은 구문(명사형) 작성, 영어는 명령조(동사원형, 첫글자 대문자)로 작성
- 본문은 영문자 72자 줄 바꿈, what 보다는 why, how, share(정보 공유)위주로 작성

#### 클론(clone)하기

> 1.  로컬의 원하는 위치에서  `git clone https://github.com/hancom-io/hwpx-owpml-model.git`  명령어를 통해 프로젝트를 복사합니다.
> 2.  이후 각 IDE를 통해 해당 프로젝트를 오픈합니다.



#### 브랜치 만들기

> 1.  로컬에서 개발을 진행하기 전 main 브랜치가 아닌 곳에서 개발을 진행하기 위해(개발 도중 코드를 다시 돌려야 할 경우를 대비하여) 새로운 브랜치를 만들고 checkout합니다.  `git checkout -b [생성할 브랜치 이름]`
> 2.  또한 원격 저장소에 코드를 푸시할 때도 다른 브랜치를 거쳐 main 브랜치에 최종 반영하기 위해 새로운 브랜치를 만들어 푸시해줍니다.  `git push origin [생성할 브랜치 이름]`
> 3.  즐겁게 개발을 진행합니다.

#### 푸시(Push)하기

> 1.  `git status`를 사용해 커밋할 코드가 있는지 확인합니다.
> 2.  커밋할 코드가 있다면  `git add [디렉토리 or 파일]`  명령어를 사용하여 staging area에 변경된 코드를 올립니다.
> 3.  `git commit -m "#[연결할 이슈 번호]커밋 메세지"`  명령어를 통해 로컬 저장소에 코드를 커밋합니다.(이슈는 저장소 이슈 탭에서 생성 및 확인이 가능합니다.)
> 4.  `git push origin [main을 제외한 다른 branch]`을 통해 원격 저장소에 코드를 푸시합니다.
> 5.  원격 저장소 main 브랜치에 반영하기 위해  `Pull requests`를 작성합니다.
> 6.  코드 검사가 완료되어 main 브랜치에 반영 된다면 성공🎉

## Contact
- 문의사항은 jjeong@hancom.com로 연락주세요.
