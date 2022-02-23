# Sobre o projeto
Esse é um projeto de automação residêncial, usando uma placa nodemcu ESP8266, ReactNative, Socket.io e Node.JS, o Desafio do projeto foi integrar socket.io com arduino e ReactNative para obtermos uma comunicação em tempo real, onde é possivel acionar ou desacionar módulos relés e ainda receber sinais resistivos ou pulsivos de forma dinâmica, sendo totalmente controlada e configurada por um aplicativo mobile.

## Automação residêncial
Automação residencial é aplicar todas as tecnologias existentes dentro de casa para facilitar tarefas que antes dependiam somente do morador. A automação tem como objetivo facilitar ou inovar em processos.

## Home Future
Decidimos nomear o projeto como Home Future para se referir a uma "Casa Futuristica", que na minha visão seria um ambiente tecnológico e inteligente.

## Instalação
Para utilizar o HomeFuture precisaremos:
  **1. ESP8266 (com projeto carregado)**
  **2. Módulo relé**
  **3. App HomeFuture instalado em um aparelho (repositório git disponível no perfil)**
  **4. Rede Wifi**
  **5. Servidor Node rodando (com acesso externo)**

## Inicialização
**Ao carregar o projeto no arduino é necessário configurar os dados de rede e servidor.**
Para entrar em modo configuração manteremos pressionado por 10 segundos o botão **flash** da placa ESP8266.
Quando o led da placa (GPIO2) piscar estaremos no **Modo Programação**.
Ao entrar em modo programação é gerado uma Rede Wifi(acess point)
Inicialmente com as seguintes configurações
|  **SSID**     |  ***Homefuture*** |
| ---    | ---       |
|  **Password** |  ***12345678***   |

Assim que conectado a rede qualquer acesso a internet através de uma URL 'http' redicionará para a pagina de configuração

![*Imagem da página de configuração*](https://i.ibb.co/wzL2fTC/config-screen.png)

***OS ITENS "DOMINIO SERVIDOR" e "PORTA SERVIDOR" DEVEM SER PRÉ CONFIGURADOS ANTES DE CARREGAR O PROJETO***

|   Nome   |  Dado  | Requisito |
| ---  | ---  | ---  |
|  **Nome wifi (SSID)** |  Nome da rede wifi para placa se conectar  |  Obrigatório* |
|  **Senha wifi (PASS)** |  Senha da rede wifi para placa se conectar  |  Obrigatório* |
|  **Dominio Servidor** |  Dominio onde esta hospedado servidor node com o Server-Side Socket.io  |  Pré-configurado (deixar em branco para manter) |
|  **Porta Servidor** |  Porta do servidor node com o Server-Side Socket.io (normalmente 443)  |  Pré-configurado (deixar em branco para manter) |
|  **Nome ponto de acesso** |  Nome do ponto de acesso para a rede de configurção da placa  |  Pré-configurado (deixar em branco para manter) |
|  **Senha ponto de acesso** |  Senha do ponto de acesso para a rede de configurção da placa  |  Pré-configurado (deixar em branco para manter) |

Após configurar clicando em "Salvar" a placa sai de Modo Programação automaticamente
(Caso queira sair do Modo Programação sem salvar as configurações, clique *5 vezes* repetidamente no botão **FLASH** da placa)
Quando fechado o Modo Programação a placa informa com 4 sinais de led

## Utilização
Abrindo pela primeira vez o Aplicativo Mobile vemos a tela de Login

![*Imagem da página de login](https://i.ibb.co/NSCJWHS/login-screen.png)

Para criar uma conta entramos em "Cadastrar-se"

![*Imagem da página de cadastro](https://i.ibb.co/HdjjqqQ/singuo-screen.png)

Basta preencher os dados para cadastro e concluir para entrar na tela principal

## Controle de Versão
Versão atual do projeto: V1.0.1

 #### .ino
  - [x] 1.0.0
  - [x] 1.0.1
  - [ ] 1.x.x
 
 #### App
  - [x] 1.0.0
  - [ ] 1.x.x

 #### Server
  - [x] 1.0.0
  - [ ] 1.x.x

 #### Banco de dados
  - [x] 1.0.0
  - [ ] 1.x.x

## Resumo
Com esse projeto pretendo a chegar a uma aplicação hibrida e fluida, com funcionamento perfeito e útil.

## Autor
### Simon Lopes
  
  - Linkedin: [simon-lopes](https://www.linkedin.com/in/simon-lopes/)
  - Twitter: [@simon_lopess](https://twitter.com/simon_lopess)
  - Instagram: [@simon_lops](https://www.instagram.com/simon_lops/)
  - Discord: L045#4789


## License
[MIT](https://choosealicense.com/licenses/mit/)
