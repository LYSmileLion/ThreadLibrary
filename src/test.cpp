#include <stdio.h>
#include <pwd.h>
#include <unistd.h>
#include <unistd.h>
int main(void)
{
    char name[100];
    gethostname(name, 100);
    printf("当前登陆的主机为：%s\n", name);
    struct passwd *pwd;
    pwd = getpwuid(getuid());
    printf("当前登陆的用户名为：%s\n", pwd->pw_name);
    return 0;
}

