using JuMP
using GLPK
using Dates

inst_path = "/Users/bernardo/Documents/codelab/Trabalho-Otimizacao/instancias/";
insts = ["bpwt_500_" "bpwt_1000_" "bpwt_2000_"];

for inst = insts
    for id = 0:3

        start = now();
        print("Time started: " * string(start) * "\n");

        mdl = Model(with_optimizer(GLPK.Optimizer, tm_lim=1));

        # Le dados do arquivo de entrada
        filename = inst * string(id);
        filepath = inst_path * filename * "/" * filename * ".dat";
        f = open(filepath);

        print("Running $filename... ");

        m = parse(Int32, readline(f));  # Numero de maquinas
        n = parse(Int32, readline(f));  # Numero de tarefas
        d = parse(Int32, readline(f));  # Due date

        # Valores Pi (duracoes das tarefas)
        P = array = Array{Int32}(undef, n);
        for i = 1: n
            P[i] = parse(Int32, readline(f));
        end

        close(f);

        @variable(mdl, S[1:n] >= 0, Int);
        @variable(mdl, M[1:m, 1:n], Bin);
        @variable(mdl, C[1:n, 1:n], Bin);

        @constraints(mdl,
            begin
                [i = 1:n], sum(M[j,i] for j = 1: m) == 1 # 1 e somente 1 maquina executa certa tarefa
                [i = 1:n], S[i] + P[i] <= d              # Todas tarefas acabam antes do Deadline
            end
        )

        for j = 1:m
            for x = 1:n
                for y = 1:n
                    if x != y

                        if M[j,x] == 1 && M[j,y] == 1
                            @constraint(mdl, (1 / d) *  ((S[x] + P[x]) - S[y]) >= C[x,y])
                        end

                        @constraint(mdl, M[j,x] + M[j,y] + C[x,y] + C[y,x] <= 3)

                    end
                end
            end
        end

        @objective(mdl, Min, sum((d - (S[i] + P[i])) for i = 1: n));

#         print(mdl);

        print("Solving... ");
        t = optimize!(mdl);
        print("Done.\n");

        print("Value for $filename: ");
        print(objective_value(mdl));
        print("\n");

#         print("S:\n");
#         print(map(value, S));
#         print("\n");
#         print("M:\n");
#         print(map(value, M));
#         print("\n");

        min_elapsed = (now() - start).value / (1000 * 60);
        print("Time elapsed: " * string(min_elapsed) * " minutes\n\n");
    end
end
