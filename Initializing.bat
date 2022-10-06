@echo off
REM /*
REM * Copyright 2022 Hancom Inc. All rights reserved.
REM  *
REM  * Licensed under the Apache License, Version 2.0 (the "License");
REM  * you may not use this file except in compliance with the License.
REM  * You may obtain a copy of the License at
REM  *
REM  * http://www.apache.org/licenses/LICENSE-2.0
REM  *
REM  * Unless required by applicable law or agreed to in writing, software
REM  * distributed under the License is distributed on an "AS IS" BASIS,
REM  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
REM  * See the License for the specific language governing permissions and
REM  * limitations under the License.
REM  */

SET BASEPATH="opensource"
SET OWPMLPATH="hwpx-owpml-model"
SET RAPIDJSONPATH="rapidjson"

REM 2022/10/04
SET OWPML_RESET_ID="232b9b463361ce8114253d77ab6a093467d7264b"
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