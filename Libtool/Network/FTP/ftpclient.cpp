#include "common.h"

#define DEFAULT_LOGFILE "./ftp.log"

struct ftp_config
{
    //PORT or PASV
    uint8_t mode;
    uint8_t *username;
    uint8_t *passwd;
    uint16_t port;
    uint32_t ip;
};

class FtpClient
{
private:
    int ctl_sockfd;
    int data_sockfd;

    // the last send time
    int last_time;
    struct ftp_config config;

    common::logrecord *log;

    // the status of ftp-link
    int status;

public:
    explicit FtpClient(uint8_t mode,uint8_t *username, uint8_t *passwd, uint16_t port, uint32_t ip)
    {
        //setting configure paramter
        this->config.mode = mode;
        this->config.username = username;
        this->config.passwd = passwd;
        this->config.port = port;
        this->config.ip = ip;
        
        ctl_sockfd = -1;
        data_sockfd = -1;
        last_time = 0;
        status = 0;
    }

    ~FtpClient()
    {
        if(ctl_sockfd != -1)
            close(ctl_sockfd);
        if(data_sockfd != -1)
            close(data_sockfd);
    }

public:
    int FTPClient_init(char *errbuf, uint32_t errlen)
    {
        int *sock = &this->ctl_sockfd;
        struct sockaddr_in adresse;
        char reponse_serv[BUFSIZ];

        int s = 0;
        adresse.sin_family = AF_INET;
        adresse.sin_port = htons(this->config.port);
        *sock = socket(AF_INET, SOCK_STREAM, 0);
        if (*sock < 0)
        {
            sprintf(errbuf, "Error, Allowing socket is failed");
            return -1;
        }

        struct timeval timeout;
        timeout.tv_sec = SOCK_STREAM;
        timeout.tv_usec = 0;
        if (setsockopt(*sock, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout)) < 0)
        {
            return -1;
        }

        if (this->config.ip > 0)
        {
            adresse.sin_addr = *((struct in_addr *)(&this->config.ip));
            sprintf(errbuf, "Connecting to adresse %s and port %d ...\n", inet_ntoa(adresse.sin_addr), this->config.port);
            s = connect(*sock, (struct sockaddr *)&adresse, sizeof(struct sockaddr_in));
            if (s == -1)
            {
                return -1;
            }
        }

        s = recv(*sock, reponse_serv, 1024, 0);
        if (s == -1)
        {
            sprintf(errbuf,"Error reading");
            return -1;
        }
        if (get_code(reponse_serv) != 220)
        {
            sprintf(errbuf,"Error msg");
            return -1;
        }
        else
            sprintf(errbuf, "Success: %s", reponse_serv);

        char req_user[BUFSIZ] = "USER ", req_pass[BUFSIZ] = "PASS ";
        snprintf(req_user, BUFSIZ, "%s%s%s", "USER ", this->config.username, "\r\n");
        if (send(*sock, req_user, strlen(req_user), 0) == -1)
        {
            sprintf(errbuf, "Error send msg %s", req_user);
            return -1;
        }
        else
        {
            s = recv(*sock, reponse_serv, 1024, 0);
            if (s == -1)
            {
                return -1;
            }
            reponse_serv[s] = '\0';
            if (get_code(reponse_serv) != 331)
            {
                if (get_code(reponse_serv) == 530)
                   sprintf(errbuf, "This server is anonymous only");
            
                return -1;
            }
            else
            {
                printf("%s", reponse_serv);
                snprintf(req_pass, BUFSIZ, "%s%s%s", "PASS ", this->config.passwd, "\r\n");
                if (send(*sock, req_pass, strlen(req_pass), 0) == -1)
                {
                    perror("Erreur ecriture");
                    return -1;
                }
                else
                {
                    s = recv(*sock, reponse_serv, 1024, 0);
                    reponse_serv[s] = '\0';
                    if (s == -1)
                    {
                        return -1;
                    }

                    if (get_code(reponse_serv) != 230)
                    {
                        if (get_code(reponse_serv) == 530)
                            printf("Login incorrect.\n");
                        return -1;
                    }
                    else
                    {
                        printf("%s", reponse_serv);
                        printf("Authentification ftp reussie\n");
                    }
                }
            }
        }

        return 0;
    }
};
