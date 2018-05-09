# Solved tasks for course "Unix SVR4 System Calls and Libraries"
___
## Дисклеймер 
ДАННЫЕ РЕШЕНИЯ ПРЕДОСТАВЛЯЕТСЯ «КАК ЕСТЬ», БЕЗ КАКИХ-ЛИБО ГАРАНТИЙ, ЯВНО ВЫРАЖЕННЫХ ИЛИ ПОДРАЗУМЕВАЕМЫХ, ВКЛЮЧАЯ ГАРАНТИИ ПРИГОДНОСТИ ДЛЯ СДАЧИ ВАШЕМУ СЕМИНАРИСТУ, НО НЕ ОГРАНИЧИВАЯСЬ ЕЮ. 

## FAQ
#### Оно собирается?
Все программы компилировались `gcc` с ключами `-Wall` и -`std=gnu99` на **Solaris** и **Ubuntu**
#### Где посмотреть сами задачи и теорию по ним?
Задачи [тут](http://ccfit.nsu.ru/~deviv/courses/unix/tasks.html), а полезности [тут](http://ccfit.nsu.ru/~deviv/courses/unix/unix/menu.html)
#### Как удобнее работать на солярке, обязательно ставить Solaris себе в дуалбут?
Завтра ищешь у себя команду `sshfs`. Похуй если ничего не поймешь. Затем идешь и узнаешь свой логин и пароль от солярки. Когда узнаешь, вбивай в терминал 

```
sshfs username@solarka.ccfit.nsu.ru:/home/students/[group]/username /home/username/localdir
```

Как смонтируешь свою соляровскую папку себе на комп, чтобы висела на рабочем столе, можешь идти дальше - тебя ждет увлекательный мир *Drag-and-drop*'a. Проводник, сверхбыстрые перетаскивания папочек, редактирование в **Visual Studio** или **VSCode**. **УДАЛЕНО** хиккующих **УДАЛЕНО** / просто неудачников типа тех, что ставят соляру в дуалбут или vim/nano-**УДАЛЕНО**, которые **УДАЛЕНО** по жизни не заставит себя ждать и уже через пол секунды ты будешь перетаскивать столько папочек, что любой адепт терминала будет улетать на реактивной тяге при одном упоминании твоей продуктивности.(Актуально для *nix систем. Для винды это возможно, но требует больше пердолинга)
Либо подключитесь к соляре с помощью своего редактора/IDE/файлового менеджера. Например Vim, Sublime, VSCode, CLion, из ФМ midnight commander и SpaceFM.
Могут возникнуть проблемы, если вы работаете не из сети НГУ(общаги и сама шарага), т.к. соляра находится во внутреней сети и без пердолинга вы сможете зайти только через ccfit.nsu.ru. У соляры и ccfit'а общий home поэтому можете редактировать на ccfit'е
#### Почему нет * номер лабы *?
На нашем потоке было достаточно сдать 23 лабы.
#### Исправления и дополнения
Если у вас есть желание исправить или дополнить существующие решения, то форкайте репозиторий и присылайте пулл реквест.

###Небольшой гуйд, как работать прямо на соляре.
Если нужно быстро что-то пофиксить, можно сделать это сразу на соляре.
Пишите Makefile, в большенстве лаб достаточно только одного "main" файла, поэтому одного Makefile'а хватит для большенства лаб.
Не ищите make. Используйте dmake. Можно сделать alias.
Для редактирования используйте ~~nano~~ vim. 
Добавте это в файл ~/.vimrc.

Подсветка синтаксиса
syntax on

Настройка tab'ов. Опционально.
set expandtab
set smarttab
set tabstop=4 
set shiftwidth=4

Подавить разворачивание tab'ов в Makefile 
autocmd FileType make setlocal noexpandtab

set cident

set nu
set sm
set insecsearch

colorsheme ${ВашаЛюбимаяСхема}

Посмотреть все цветовые схемы, можно набрав `: colorsheme` и листать нажимая на Tab.

Можно добавить fold'ы(свёртки) и прочие плюшки, но надо пердолиться.   
После редактирования .vimrc наберите `:so%`, чтобы vim вычитал конфиг.
P.S. можно бесконечно пердолится и ~~дрочить~~ настраивать vim под себя.**TAKE IT BOY** 
Используйте сеть Internet, если что-то не понятно.
