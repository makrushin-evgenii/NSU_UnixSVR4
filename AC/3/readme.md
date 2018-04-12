1. Исполните программу и посмотрите вывод:
banyrule $ chmod 600 file
banyrule $ ./a.out file
> uid=1000, euid=1000
> 1 try: accessed
> uid=1000, euid=1000
> 2 try: accessed

2. Сделайте программу доступной для запуска членам вашей группы и пусть ваши одногруппники исполнят программу:
> tempusr_1 $ ./a.out file
> uid=1001, euid=1001
> 1 try: access denied
> ./a.out: Permission denied

3. Командой chmod u+s prog установите бит установки идентификатора пользователя и пусть ваши одногруппники опять исполнят эту программу.
banyrule $ chmod u+s a.out
tempusr_1 $ ./a.out file
> uid=1001, euid=1000
> 1 try: accessed
> uid=1001, euid=1000
> 2 try: accessed

