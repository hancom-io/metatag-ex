# MetatagEX
Metattg Extract, Sort

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
====================================================
* 사용된 라이브러리
- xerces-c(Apache License 2.0)
- rapidjson(MIT)
- libarchive(new BSD)

====================================================
