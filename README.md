# Xenon-OS

**Xenon-OS** — это проект операционной системы с нуля, созданный в учебных и эксперементальных целях. 

## Основные особенности

Проект подразумевает поэтапное расширение функционала, что позволит внедрять уникальные особенности
и улучшения по мере углубления знаний в области разработки ОС. /

...

## Project structure

```
./
├── arch
│   └── i386
├── configs (grub.cfg, etc)
├── docs (tech, tools)
├── kernel
├── libc
└── src
```

## Начало работы

...

## Usage
1. Clone the project:
```git clone https://github.com/singl3focus/xenon-os```

2. Change dir and run scripts:
```./make_clear.sh && ./make.sh```

TODO (ignore it now):
```
### Makefile 

Make command form:
```
[target]: [dependes]
       [command]
```

1. For collect ISO and start it to Bochs emulator: \
```make run```
```

3. Start OS with emulation:
   - QEMU: ```qemu-system-i386 -cdrom xenon-os.iso```
   - BOCHS: ```TODO```

> Use it!


### Git navigation

1. **Иерархия веток**:
   - `main` ← `develop` ← `feature/*`/`bugfix/*`
   - Каждая функция или исправление изолированы в своей ветке.

   - **Связанность**:
      - **Feature-ветки** интегрируются через Pull Requests в `develop` после code review.
      - **Bugfix-ветки** для `main` выпускаются как патчи, а для `develop` — как улучшения.
      - Релизы из `develop` в `main` помечаются тегами (v0.1.0, v0.2.0 и т.д.).
 
   - **Пример**:
      - Разработка драйвера USB:
      - Создана ветка `feature-usb-driver` из `develop`.
      - После завершения — PR в `develop`.
      - Тестирование в `develop` → слияние в `main` при релизе.

2. Commits

|Обозначение|Описание|
|-----------|--------|
|major|Добавление блока, который значительно меняет поведение ПО|
|feat|Новая функциональность (например, добавление драйвера VGA)|
|fix|Исправление ошибки (например, устранение зависания при обработке прерываний)|
|chore|Незначительные изменения, к примеру, обновление в документации|

## Contributors (a.k.a. Authors):

- Anton Smirnov (ascom8)
- Imran Tursunov (singl3focus)

