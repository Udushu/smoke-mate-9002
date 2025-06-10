@echo off
SETLOCAL

REM === Configuration ===
set IMAGE_NAME=smokemate-image
set CONTAINER_NAME=smokemate-container
set PORT=5000

REM === Build the Docker image ===
echo Building Docker image "%IMAGE_NAME%"...
docker build -t %IMAGE_NAME% .

IF ERRORLEVEL 1 (
    echo Docker build failed.
    EXIT /B 1
)

REM === Stop and remove any previous container with the same name ===
echo Removing old container (if exists)...
docker stop %CONTAINER_NAME% >nul 2>&1
docker rm %CONTAINER_NAME% >nul 2>&1

REM === Run the container ===
echo Starting container "%CONTAINER_NAME%" on port %PORT%...
docker run -it -p %PORT%:%PORT% --name %CONTAINER_NAME% %IMAGE_NAME%

ENDLOCAL
echo Container "%CONTAINER_NAME%" is running on port %PORT%.
echo To stop the container, run: docker stop %CONTAINER_NAME%