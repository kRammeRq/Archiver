# Archiver
Простой архиватор, который может архивировать и распаковывать один файл
Установка и сборка
Установите CMake и zlib.
Склонируйте проект:
   ```bash
   git clone <url>
   cd <project directory>
Создайте директорию для сборки и соберите проект:
    mkdir build
    cd build
    cmake ..
    cmake --build .
Использование:
Чтобы создать архив ./MyArchiver a <original_file> <archive_file>
Чтобы распаковать архив ./MyArchiver e <archive_file> <original_file>
Пример команд
C:\Users\User1\source\repos\archiver\x64\Debug\archiver.exe e C:\Users\User1\Desktop\archive.z C:\Users\User1\Desktop\original_restored.txt
C:\Users\User1\source\repos\archiver\x64\Debug\archiver.exe a C:\Users\User1\Desktop\original.txt C:\Users\User1\Desktop\archive.z
