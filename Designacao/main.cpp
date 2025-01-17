/*--------------------- File: main.cpp  ----------------------+
|Modelo PLI - Problema da Designação                          |
|					      		                              |
|					      		                              |
| Implementado por Bárbara Pereira Medeiros Dias              |
+-----------------------------------------------------------+ */

#include <bits/stdc++.h>
#include <ilcplex/ilocplex.h>

using namespace std;
ILOSTLBEGIN

int n;  // Número de pessoas/tarefas
vector<vector<int>> custos;  // Matriz de custos

// Função que configura e resolve o modelo CPLEX
void cplex() {
    IloEnv ambiente;  // Ambiente CPLEX

    // Variáveis de decisão
    IloArray<IloBoolVarArray> x(ambiente, n);
    for (int i = 0; i < n; i++) {
        x[i] = IloBoolVarArray(ambiente, n);
    }

    // Modelo
    IloModel modelo(ambiente);
    IloExpr objetivo(ambiente);

    // Função objetivo: minimizar o custo total das designações
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            objetivo += custos[i][j] * x[i][j];
        }
    }
    modelo.add(IloMinimize(ambiente, objetivo));  // Adiciona a função objetivo

    // Restrições: cada pessoa deve ser designada a exatamente uma tarefa
    for (int i = 0; i < n; i++) {
        IloExpr sum(ambiente);
        for (int j = 0; j < n; j++) {
            sum += x[i][j];
        }
        modelo.add(sum == 1);
    }

    // Restrições: cada tarefa deve ser designada a exatamente uma pessoa
    for (int j = 0; j < n; j++) {
        IloExpr sum(ambiente);
        for (int i = 0; i < n; i++) {
            sum += x[i][j];
        }
        modelo.add(sum == 1);
    }

    // Execução
    IloCplex solver(modelo);  // Configura o solver CPLEX

    time_t inicio, fim;
    string status_solucao;

    time(&inicio);
    solver.solve();  // Executa a resolução do modelo
    time(&fim);

    // Resultados
    bool solucao_existe = true;
    switch (solver.getStatus()) {
        case IloAlgorithm::Optimal:
            status_solucao = "Ótima";
            break;
        case IloAlgorithm::Feasible:
            status_solucao = "Factível";
            break;
        default:
            status_solucao = "Sem Solução";
            solucao_existe = false;
    }

    cout << endl << "Status da solução: " << status_solucao << endl << endl;

    if (solucao_existe) {
        // Valor da função objetivo
        IloNum valor_objetivo = solver.getObjValue();

        // Imprime os valores das variáveis de decisão
        cout << "Designação (pessoa -> tarefa):" << endl;
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                if (solver.getValue(x[i][j]) > 0.5) {  // Se a variável for 1 (binária)
                    cout << "Pessoa " << i + 1 << " -> Tarefa " << char('A' + j) << endl;
                }
            }
        }
        cout << "Função Objetivo Valor = " << valor_objetivo << endl;

        std::cout << std::fixed;
        std::cout << std::setprecision(6);
        cout << "(" << difftime(fim, inicio) << " segundos)" << endl;
    }

    // Liberação de memória
    modelo.end();
    ambiente.end();
}

// Função principal
int main() {
    ifstream entrada("in.txt");  // Arquivo de entrada

    // Lê o número de pessoas e tarefas
    entrada >> n;

    // Inicializa a matriz de custos
    custos.assign(n, vector<int>(n));

    // Lê os dados de entrada
    int pessoa, custo;
    char tarefa;
    while (entrada >> pessoa >> tarefa >> custo) {
        custos[pessoa - 1][tarefa - 'A'] = custo;
    }

    // Executa o CPLEX
    cplex();
    return 0;
}
