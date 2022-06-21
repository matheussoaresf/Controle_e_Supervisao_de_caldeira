# Controle e supervisão de um sistema de caldeira simulado

# Resumo
Tanques são projetados para controlar e supervisionar processos industrias, afim de manter parâmetros com temperatura e nível controlados para a geração de um produto ou subproduto. Nesses casos, o controle deve ocorre em tempo real por meio da captação de sensores e pela ação de atuadores. Surgindo assim,  requisitos temporais que devem ser supridos por meio dos softwares em tempo real atrelados a sistemas computacionais capazes de compreender tais requisitos. Nele, o controle da temperatura no interior da caldeira deve se limitar a temperatura de referência definida pelo usuário e o controle de nível a altura pré estabelecida. Ambos os controles utilizam-se de parâmetros como fluxo de vasão de entrada, fluxo de vasão de saída, temperatura e altura, captados por sensores com periodicidade, atualidade, deadline e simultaneidade definidos.  

# Introdução
	
Software em tempo real, pode ser entendido como sistemas computacionais com requisitos de tempo real, sendo uma categoria especial de sistemas operacionais. Essa técnica, aplica-se a problemas que exijam requisitos com funcionalidades de natureza temporal não triviais, nesse sentido é essencial a confiabilidade no tempo de execução da tarefa e compatibilidade com prazos e requisitos definidos, ocasionando em resultados corretos logicamente e temporalmente[1].
	
Para os requisitos temporais, é necessário destacar que estão diretamente acoplados ao meio, no sentido de estarem fortemente associados com o mundo físico e que por isso a extração dos requisitos devem ocorrer na perspectiva do sistema responder ao estímulos do ambiente. Destacam-se os seguintes requisitos temporais:  periodicidade, Deadline, atualidade ou frescor dos dados e a simultaneidade dos dados[1].
	
Na aplicação de softwares de tempo real, a sincronização e a comunicação entre tarefas pode ocasionar inconsistência de dados e inversões de prioridades entre tarefas caso seja mal implementado. Para os trechos do código onde a tarefa acessa algum recurso compartilhado entre várias tarefas, é necessário definir uma seção crítica. Para evitar a inconsistência dos dados é usado o mecanismos do Mutex, "Técnica de sincronização que possibilita assegurar o acesso exclusivo (leitura e escrita) a um recurso compartilhado por duas ou mais entidades"[2], evitando que duas tarefas acessem ao mesmo tempo a memória. 
	
Nesse sentido, para acessar recursos compartilhados por outras tarefas é necessário definir mecanismos de sincronização para controlar o acesso de recursos e garantir que todos as tarefas possam executar suas seções críticas sem interferir com as seções críticas das outras tarefas.
	
Para programas concorrentes o problema da seção crítica é mais frequente, mas também existem  situações onde é necessário sincronizar tarefas que colaborem atráves de variáveis compartilhadas, nesses casos, usar apenas a exclusão mutua e Mutex pode gerar mais problemas. Portanto, é necessário definir outros padrões de sincronização como o semáforo e o monitor que são padrões para a programação concorrente com variáveis compartilhadas entre tarefas.
	
Dessa maneira, os monitores são módulos onde as interações entre tarefas acontecem. Para os monitores, a idéia é tornar determinados recursos privativo as tarefas[1]. Nesse sentido, o monitor tenta quebrar o programa e recursos em módulos que são acessado as tarefas.
