/**
Aluno: Matheus Paul Lopuch
TDE-1, MÁQUINAS DE ESTADO FINITO E GRAMÁTICAS

a) 𝐿0 = {𝑥 | 𝑥 ∈ {0,1}∗ 𝑒 𝑐𝑎𝑑𝑎 0 𝑒𝑚 𝑥 é 𝑠𝑒𝑔𝑢𝑖𝑑𝑜 𝑝𝑜𝑟 𝑝𝑒𝑙𝑜 𝑚𝑒𝑛𝑜𝑠 𝑢𝑚 1};
b) 𝐿1 = {𝑥 | 𝑥 ∈ {0,1}∗ 𝑒 𝑥 𝑡𝑒𝑟𝑚𝑖𝑛𝑎 𝑐𝑜𝑚 00};
c) 𝐿2 = {𝑥 |𝑥 ∈ {0,1}∗ 𝑒 𝑥 𝑐𝑜𝑛𝑡é𝑚 𝑒𝑥𝑎𝑡𝑎𝑚𝑒𝑛𝑡𝑒 3 𝑧𝑒𝑟𝑜𝑠};
d) 𝐿3 = {𝑥 |𝑥 ∈ {0,1}∗ 𝑒 𝑥 𝑖𝑛𝑖𝑐𝑖𝑎 𝑐𝑜𝑚 1};
e) 𝐿4 = {𝑥 |𝑥 ∈ {0,1}∗ 𝑒 𝑥 𝑛ã𝑜 𝑐𝑜𝑚𝑒ç𝑎 𝑐𝑜𝑚 1}; 

Implemente, utilizando a linguagem C um programa capaz de implementar cada uma
destas máquinas na plataforma Arduíno. Neste caso, as strings de entrada deverão
ser configuradas manualmente e a identificação, ou não, deverá ser feita por um led. 
 */

// Pino para o LED
#define LED_PIN 13  

// Definição dos estados para cada máquina
// MEFD-0
#define L0_S0 0  // Estado inicial e de aceitação
#define L0_S1 1  // Estado não-aceitante

// MEFD-1
#define L1_S0 0  // Estado inicial
#define L1_S1 1  // Estado intermediário
#define L1_S2 2  // Estado de aceitação

// MEFD-2
#define L2_S0 0  // Estado inicial
#define L2_S1 1  // Estado após 1 zero
#define L2_S2 2  // Estado após 2 zeros
#define L2_S3 3  // Estado após 3 zeros (aceitação)
#define L2_FINAL 4  // Estado morto

// MEFD-3
#define L3_S0 0  // Estado inicial
#define L3_S1 1  // Estado de aceitação
#define L3_FINAL 2  // Estado morto

// MEFD-4
#define L4_S0 0  // Estado inicial
#define L4_S1 1  // Estado de aceitação
#define L4_FINAL 2  // Estado morto

// Variáveis globais
int currentMachine = 0;  // Máquina selecionada (0-4)
int currentState = 0;    // Estado atual da máquina selecionada
bool accepted = false;   // Indica se a string atual é aceita
char serialBuffer[64];   // Buffer para entrada via Serial
int bufferIndex = 0;     // Índice atual no buffer

// Nomes das máquinas para exibição no Serial
const char* machineNames[] = {
  "L0: cada 0 é seguido por pelo menos um 1",
  "L1: termina com 00",
  "L2: contém exatamente 3 zeros",
  "L3: inicia com 1",
  "L4: não começa com 1"
};

// Strings de teste para cada máquina (exemplos de strings aceitas)
const char* testStrings[] = {
  "010111",   // L0: cada 0 é seguido por pelo menos um 1
  "100",      // L1: termina com 00
  "10100",    // L2: contém exatamente 3 zeros
  "101",      // L3: inicia com 1
  "011"       // L4: não começa com 1
};

// Strings de teste negativas para cada máquina (exemplos de strings rejeitadas)
const char* negativeTestStrings[] = {
  "10110",    // L0: contém um 0 não seguido por 1
  "10",       // L1: não termina com 00
  "10000",    // L2: contém mais de 3 zeros
  "011",      // L3: não inicia com 1
  "101"       // L4: começa com 1
};

void setup() {
  // Configuração do pino do LED
  pinMode(LED_PIN, OUTPUT);
  
  // Inicializa a comunicação serial
  Serial.begin(9600);
  
  // Mostra o menu inicial
  showMainMenu();
}

void loop() {
  // Verifica se há dados disponíveis na Serial
  while (Serial.available() > 0) {
    char inChar = Serial.read();
    
    // Se receber Enter (carriage return ou newline), processa o comando
    if (inChar == '\r' || inChar == '\n') {
      if (bufferIndex > 0) {
        serialBuffer[bufferIndex] = '\0';  // Finaliza a string
        processCommand(serialBuffer);
        bufferIndex = 0;  // Reinicia o buffer
      }
    } else if (bufferIndex < sizeof(serialBuffer) - 1) {
      serialBuffer[bufferIndex++] = inChar;  // Adiciona o caractere ao buffer
    }
  }
}

// Exibe o menu principal
void showMainMenu() {
  Serial.println("\n==== MÁQUINAS DE ESTADO FINITO (MEFD) ====");
  Serial.println("Selecione uma opção:");
  Serial.println("1-5: Selecionar máquina (1 = L0, 2 = L1, etc.)");
  Serial.println("t: Testar a máquina atual com string de exemplo que DEVE ser aceita");
  Serial.println("n: Testar a máquina atual com string de exemplo que NÃO deve ser aceita");
  Serial.println("s: Inserir sua própria string para testar");
  Serial.println("m: Mostrar este menu");
  Serial.println("i: Informações sobre a máquina atual");
  Serial.println("=====================================");
  
  // Mostra a máquina atual
  Serial.print("Máquina atual: ");
  Serial.println(machineNames[currentMachine]);
}

// Processa um comando recebido via Serial
void processCommand(const char* cmd) {
  // Comando de um único caractere
  if (strlen(cmd) == 1) {
    char command = cmd[0];
    
    switch (command) {
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
        // Seleciona a máquina (1-5 correspondem a L0-L4)
        currentMachine = command - '1';
        Serial.print("Máquina selecionada: ");
        Serial.println(machineNames[currentMachine]);
        resetMachine();
        break;
        
      case 't':
        // Testa com string positiva
        Serial.print("Testando string que deve ser ACEITA: ");
        Serial.println(testStrings[currentMachine]);
        testString(testStrings[currentMachine]);
        break;
        
      case 'n':
        // Testa com string negativa
        Serial.print("Testando string que deve ser REJEITADA: ");
        Serial.println(negativeTestStrings[currentMachine]);
        testString(negativeTestStrings[currentMachine]);
        break;
        
      case 'm':
        // Mostra o menu
        showMainMenu();
        break;
        
      case 'i':
        // Mostra informações da máquina atual
        showMachineInfo();
        break;
        
      case 's':
        // Solicita uma string para testar
        Serial.println("Digite uma string de 0s e 1s para testar (seguida de Enter):");
        break;
        
      default:
        // Comando não reconhecido
        Serial.println("Comando não reconhecido. Digite 'm' para ver o menu.");
        break;
    }
  } else if (cmd[0] == 's' && cmd[1] == ' ') {
    // Comando para testar uma string específica
    testString(cmd + 2);  // Pula o "s " inicial
  } else {
    // Verifica se a string contém apenas 0s e 1s
    bool validBinary = true;
    for (int i = 0; cmd[i] != '\0'; i++) {
      if (cmd[i] != '0' && cmd[i] != '1') {
        validBinary = false;
        break;
      }
    }
    
    if (validBinary) {
      // Testa a string binária
      Serial.print("Testando string personalizada: ");
      Serial.println(cmd);
      testString(cmd);
    } else {
      Serial.println("Comando não reconhecido. Digite 'm' para ver o menu.");
    }
  }
}

// Mostra informações detalhadas sobre a máquina atual
void showMachineInfo() {
  Serial.println("\n--- Informações da Máquina ---");
  Serial.print("Máquina: ");
  Serial.println(machineNames[currentMachine]);
  
  Serial.println("Descrição da linguagem reconhecida:");
  switch (currentMachine) {
    case 0:
      Serial.println("L0 = {x | x ∈ {0,1}* e cada 0 em x é seguido por pelo menos um 1}");
      Serial.println("Exemplos aceitos: 010111, 1111, 01110111011");
      break;
    case 1:
      Serial.println("L1 = {x | x ∈ {0,1}* e x termina com 00}");
      Serial.println("Exemplos aceitos: 00, 100, 11100");
      break;
    case 2:
      Serial.println("L2 = {x | x ∈ {0,1}* e x contém exatamente 3 zeros}");
      Serial.println("Exemplos aceitos: 000, 010100, 111000111");
      break;
    case 3:
      Serial.println("L3 = {x | x ∈ {0,1}* e x inicia com 1}");
      Serial.println("Exemplos aceitos: 1, 10, 1110");
      break;
    case 4:
      Serial.println("L4 = {x | x ∈ {0,1}* e x não começa com 1}");
      Serial.println("Exemplos aceitos: ε (string vazia), 0, 01110");
      break;
  }
  
  Serial.println("\nUse os comandos:");
  Serial.println("'t' para testar com um exemplo positivo");
  Serial.println("'n' para testar com um exemplo negativo");
  Serial.println("'s' seguido da sua string para testar uma entrada personalizada");
}

// Testa uma string específica na máquina atual
void testString(const char* str) {
  resetMachine();
  
  Serial.print("Processando: ");
  
  // Processa cada símbolo da string
  for (int i = 0; str[i] != '\0'; i++) {
    int input = str[i] - '0';  // Converte de char para int (0 ou 1)
    Serial.print(input);
    processInput(input);
  }
  
  // Exibe o resultado
  Serial.print(" -> ");
  if (accepted) {
    Serial.println("ACEITO ✓");
  } else {
    Serial.println("REJEITADO ✗");
  }
  
  // Mantém o LED indicando o estado final
  updateLED();
}

// Reinicia a máquina selecionada
void resetMachine() {
  switch (currentMachine) {
    case 0:  // L0
      currentState = L0_S0;
      accepted = true;  // Estado inicial é de aceitação
      break;
    case 1:  // L1
      currentState = L1_S0;
      accepted = false;  // Estado inicial não é de aceitação
      break;
    case 2:  // L2
      currentState = L2_S0;
      accepted = false;  // Estado inicial não é de aceitação
      break;
    case 3:  // L3
      currentState = L3_S0;
      accepted = false;  // Estado inicial não é de aceitação
      break;
    case 4:  // L4
      currentState = L4_S0;
      accepted = true;  // Estado inicial é de aceitação
      break;
  }
  
  // Atualiza o LED
  updateLED();
}

// Processa uma entrada (0 ou 1) na máquina atual
void processInput(int input) {
  switch (currentMachine) {
    case 0:  // L0: cada 0 é seguido por pelo menos um 1
      processL0(input);
      break;
    case 1:  // L1: termina com 00
      processL1(input);
      break;
    case 2:  // L2: contém exatamente 3 zeros
      processL2(input);
      break;
    case 3:  // L3: inicia com 1
      processL3(input);
      break;
    case 4:  // L4: não começa com 1
      processL4(input);
      break;
  }
  
  // Atualiza o LED
  updateLED();
}

// Implementação da MEFD-0
void processL0(int input) {
  switch (currentState) {
    case L0_S0:  // Estado inicial e de aceitação
      if (input == 0) {
        currentState = L0_S1;
        accepted = false;
      }
      // Se input == 1, permanece em S0
      break;
    case L0_S1:  // Estado não-aceitante
      if (input == 1) {
        currentState = L0_S0;
        accepted = true;
      }
      // Se input == 0, permanece em S1
      break;
  }
}

// Implementação da MEFD-1
void processL1(int input) {
  switch (currentState) {
    case L1_S0:  // Estado inicial
      if (input == 0) {
        currentState = L1_S1;
      }
      // Se input == 1, permanece em S0
      accepted = false;
      break;
    case L1_S1:  // Estado intermediário
      if (input == 0) {
        currentState = L1_S2;
        accepted = true;
      } else {  // input == 1
        currentState = L1_S0;
        accepted = false;
      }
      break;
    case L1_S2:  // Estado de aceitação
      if (input == 1) {
        currentState = L1_S0;
        accepted = false;
      }
      // Se input == 0, permanece em S2 e accepted = true
      break;
  }
}

// Implementação da MEFD-2
void processL2(int input) {
  switch (currentState) {
    case L2_S0:  // Estado inicial
      if (input == 0) {
        currentState = L2_S1;
      }
      // Se input == 1, permanece em S0
      accepted = false;
      break;
    case L2_S1:  // Estado após 1 zero
      if (input == 0) {
        currentState = L2_S2;
      }
      // Se input == 1, permanece em S1
      accepted = false;
      break;
    case L2_S2:  // Estado após 2 zeros
      if (input == 0) {
        currentState = L2_S3;
        accepted = true;
      }
      // Se input == 1, permanece em S2
      break;
    case L2_S3:  // Estado após 3 zeros (aceitação)
      if (input == 0) {
        currentState = L2_FINAL;
        accepted = false;
      }
      // Se input == 1, permanece em S3 e accepted = true
      break;
    case L2_FINAL:  // Estado morto
      // Permanece no estado morto independente da entrada
      accepted = false;
      break;
  }
}

// Implementação da MEFD-3
void processL3(int input) {
  switch (currentState) {
    case L3_S0:  // Estado inicial
      if (input == 0) {
        currentState = L3_FINAL;
        accepted = false;
      } else {  // input == 1
        currentState = L3_S1;
        accepted = true;
      }
      break;
    case L3_S1:  // Estado de aceitação
      // Permanece no estado de aceitação independente da entrada
      // accepted = true (já está definido)
      break;
    case L3_FINAL:  // Estado morto
      // Permanece no estado morto independente da entrada
      // accepted = false (já está definido)
      break;
  }
}

// Implementação da MEFD-4
void processL4(int input) {
  switch (currentState) {
    case L4_S0:  // Estado inicial
      if (input == 0) {
        currentState = L4_S1;
        accepted = true;
      } else {  // input == 1
        currentState = L4_FINAL;
        accepted = false;
      }
      break;
    case L4_S1:  // Estado de aceitação
      // Permanece no estado de aceitação independente da entrada
      // accepted = true (já está definido)
      break;
    case L4_FINAL:  // Estado morto
      // Permanece no estado morto independente da entrada
      // accepted = false (já está definido)
      break;
  }
}

// Atualiza o LED de acordo com o estado atual
void updateLED() {
  digitalWrite(LED_PIN, accepted ? HIGH : LOW);
}
