## Test task | Тестовое задание

#### Short Description
A prototype system for tracking the operation of a computer club has been developed. The program processes logs from a text file, verifying them against the specified technical requirements format. It then generates outgoing events and outputs a final report detailing all events and revenue for each table.

#### Краткое описание
Разработан прототип системы, отслеживающей работу компьютерного клуба. Программа обрабатыает логи из текстового файла, сверяя с заданным техническим заданием форматом. Далее она генерирует исходящие события и выводит финальный отчёт по всем событиям и выручке по каждому столу.

#### Project tree | Дерево проекта
```
.
├── README.md
├── CMakeLists.txt
├── include
│   ├── CClubHandler.h
│   ├── CClubParser.h
│   └── models.h
├── src
│   ├── CClubHandler.cpp
│   ├── CClubParser.cpp
│   └── main.cpp
└── tests
    ├── test_handler.cpp
    └── test_parser.cpp
```

#### Software | Использованное ПО
 - C++20
 - gcc 15.2
 - OS Linux
 - STL, GoogleTest
 - CMake 3.15+

#### Building and Testing
1. Install the GoogleTest library
2. In the root directory, execute the command: ```cmake -S . -B <desired_build_directory>```
3. In the root directory, execute the command: ```cmake --build <desired_build_directory>```
4. Run:
   4.1 For tests: run ```<desired_build_directory>/cclub_tests```
   4.2 For program execution: run ```<desired_build_directory>/cclub [file].txt```

#### Сборка и тестирование
1. Установить библиотеку GoogleTest
2. В корневой директории выполнить команду ```cmake -S . -B <желаемая_директория_сборки>```
3. В корневой директории выполнить команду ```cmake --build <желаемая_директория_сборки>```
4. Запуск:
   4.1 Для тестов запустить ```<желаемая_директория_сборки>/cclub_tests```
   4.2 Для выполнения программы запустить ```<желаемая_директория_сборки>/cclub [файл].txt```

#### Tests
- ```test_parser.cpp``` contains unit tests for the ```CClubParser``` class. Verifies correct parsing and outputs error messages for invalid lines.
- ```test_handler.cpp``` contains unit tests for the ```CClubHandler``` class. Verifies correct processing of incoming events and generation of outgoing events.

#### Тесты
- ```test_parser.cpp``` содержит юнит-тесты класса ```CClubParser```. Проверяет корректность парсинга и вывод ошибочных строк.
- ```test_handler.cpp``` содержит юнит-тесты класса ```CClubHandler```. Проверяет корректность обработки входящих и генерации исходящих событий.