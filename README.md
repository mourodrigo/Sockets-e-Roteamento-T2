#Programação de Socket e Roteamento

Programação com sockets para simular/emular um protocolo de roteamento em redes. 
Executando processos representando os roteadores (nós) da rede, os quais irão trocar pacotes de roteamento via sockets UDP. 
Os nós executam o algoritmo Bellman-Ford distribuído para computar as suas tabelas de roteamento.

Utilizando-se soluções da teoria dos grafos, cada nó computa o caminho completo para todos os destinos possíveis na rede. N

A Aplicação de transmissão confiável de mensagens de texto, limitadas a 100 caracteres. A mensagem é roteada da origem até o destino
segundo o caminho computado por cada roteador.

A qualquer momento, os roteadores podem ser "ligados" ou "desligados" (criando ou matando os processos correspondentes). O roteamento deve se adaptar a estas situações. 

Após cada atualização da tabela de rotas, o roteador deve retornar a tabela no console com o timestamp da mudança. Ele também deve apresentar uma mensagem quando recebe ou envia pacotes.

Cada roteador se comunica somente com os seus vizinhos imediatos (adjacentes), utilizando os seus respectivos endereços de sockets (informação obtida do arquivo de configuração roteador.config) para enviar e receber mensag
