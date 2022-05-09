@echo off
SET BASEPATH="opensource"
SET OWPMLPATH="hwpx-owpml-model"
SET RAPIDJSONPATH="rapidjson"

REM 2022/04/28
SET OWPML_RESET_ID="60171469e4a1799edd4493f738a5e91fc7df7bba"
REM 2022/03/09
SET RAPID_RESET_ID="8261c1ddf43f10de00fd8c9a67811d1486b2c784"



REM MetatagEX 초기설정 파일입니다.
REM owpml-hwpx-model 저장소를 clone합니다.
REM rapidjson 저장소를 clone합니다. 

if not exist %BASEPATH%\ (
mkdir %BASEPATH%
)

cd %BASEPATH%


if not exist %OWPMLPATH%\ (
git clone https://github.com/hancom-io/hwpx-owpml-model.git
)

if exist %OWPMLPATH%\ (
cd  %OWPMLPATH%
git reset --hard %OWPML_RESET_ID%
)

cd ..

if not exist %RAPIDJSONPATH%\ (
git clone https://github.com/Tencent/rapidjson.git
)

if exist %RAPIDJSONPATH%\ (
cd %RAPIDJSONPATH%
git reset --hard %RAPID_RESET_ID%
)


cd ../..