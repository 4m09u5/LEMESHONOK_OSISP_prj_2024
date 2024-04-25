# BitTorrent клиент

Этот проект представляет собой простой CLI BitTorrent клиент, поддерживающий UDP и HTTP трекеры.
Работа выполнена в качестве курсового проекта по дисциплине ОСиСП.

### Как собрать этот проект?

* Склонировать репозиторий: `git clone https://github.com/4m09u5/LEMESHONOK_OSISP_prj_2024.git`
* Переместиться в корень репозитория: `cd LEMESHONOK_OSISP_prj_2024`
* Создать каталог для сборки: `mkdir build`
* Переместиться в каталог сборки: `cd build`
* Воспользоваться утилитой cmake: `cmake ..`
* Запустить сборку утилитой make: `make`

Скомпилированная программа будет находится в текущем каталоге (build).

### Как пользоваться программой?

На данный момент момент клиент поддерживает следующие режимы работы:

* `BitTorrent info [путь к торрент файлу]` - прочитать информацию из торрент файла.
* `BitTorrent download [путь к торрент файлу] [каталог загрузки]` - скачать данные, указанные в торрент файле в введённый каталог

Также описание этих режимов работы может быть получено используя команду help: `BitTorrent help`

### Примеры работы программы

Интерфейс пользователя в процессе загрузки.

![Пример загрузки](https://raw.githubusercontent.com/4m09u5/LEMESHONOK_OSISP_prj_2024/74dd4c3/download_example.png)