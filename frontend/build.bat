@echo off
set IMAGE_NAME=smokemate-frontend
set CONTAINER_NAME=smokemate-frontend-container
set HOST_PORT=3000
set CONTAINER_PORT=80

REM Build the Docker image
docker build -t %IMAGE_NAME% .

REM Check if a container with the same name is running and stop/remove it
docker ps -a --filter "name=%CONTAINER_NAME%" --format "{{.Names}}" | findstr /i "%CONTAINER_NAME%" >nul
if %errorlevel%==0 (
    echo Stopping and removing existing container: %CONTAINER_NAME%
    docker stop %CONTAINER_NAME%
    docker rm %CONTAINER_NAME%
)

REM Run the container with port mapping
docker run -d --name %CONTAINER_NAME% -p %HOST_PORT%:%CONTAINER_PORT% %IMAGE_NAME%

echo.
echo App should be available at http://localhost:%HOST_PORT%
pause