# MetatagEX
Metatg Extract, Sort

====================================================
* 요약

- hwpx 문서안에 포함된 메타태그를 추출하여 콘솔(터미널)에 보여주거나 json 파일로 저장하고 json파일을 입력받아 메타태그를 분류하여 콘솔(터미널)에 보여주거나 json파일로 저장하는 프로그램 메타태그는 한글 문서 자체나 표, 개체들에 지정될수 있다. 압축을 해제하여 생성되는 header, section 의 xml 파일을 파싱하여 태그를 추출한다(OWPML)

====================================================
* 사용법
- Windows
- ex) MetatagEX.exe sl ^.*.hwpx
- 설명 : sl(sourcelist)를 이용하여 목록을 추출하고  ^.*.hwpx 정규식에 해당하는 파일을 파싱한다.
- Linux(Ubuntu)
- ex) ./MetatagEX dl f ^.*.hwpx input.json result.json
- 설명 : dl(destlist)를 이용하여 파일을 분류하고 입력으로 input.json을 사용한다. f(file) 옵션을 통해 result.json 파일에 결과를 저장한다.

* 옵션들
- sl(sourcelist) : 목록을 추출
- dl(destlist) : 입력받은 json 파일을 이용해서 파일을 분류
- f(file) : 파일로 결과를 저장
- dsc(descend) : 내림차순으로 결과를 출력 또는 저장 default는 오름차순
- pr(progress) : 진행 Percent를 콘솔(터미널)에 출력한다. 쓰레드로 동작한다.
- doc(document) : 한글 문서 자체에 부여된 메타태그에 대해서만 수행한다.

====================================================

* 사용된 라이브러리(License)
- xerces-c(Apache License 2.0) : XML 파싱 라이브러리
- rapidjson(MIT) : json 파싱라이브러리
- libarchive(new BSD) : 압축 해제 라이브러리

====================================================

* Contributing
-요구 기술 : C/C++, Windows Programming, Linux

====================================================

* Install
- Windows : buildscript.bat 파일 실행 또는 Visual Studio를 이용하여 CMake 열기(CMakeSettings.json 설정되어있음)후 직접 빌드
- Linux(Ubuntu) : buildscript.sh 실행 또는 직접빌드
- Windows 라이브러리 경로 : out/install/x64-Debug (CMakeSettings.json 의 installRoot에 설정되어 있음)
- buildscript.bat 파일 빌드시 컴파일러는 Visual Studio 15 2017 로 설정되어 있음
- 윈도우에서 빌드하기위해서는 CMake가 설치되어 있어야함.

====================================================
