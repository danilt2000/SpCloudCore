# Используйте официальный образ GCC в качестве базового
FROM gcc:13

# Установите CMake и другие зависимости
RUN apt-get update && \
    apt-get install -y cmake make

EXPOSE 8081
# Создайте директорию для вашего проекта
WORKDIR /app

# Скопируйте исходный код в контейнер
COPY . /app

# Builing it
RUN cmake .
RUN cmake --build .
WORKDIR /app/SpCloudMain
# Укажите команду для запуска вашего приложения
CMD ["./SpCloudMain"]
