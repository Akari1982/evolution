
MutateConnectionsChance = 0.25
PerturbChance = 0.90
CrossoverChance = 0.75
LinkMutationChance = 2.0
NodeMutationChance = 0.50
BiasMutationChance = 0.40
StepSize = 0.1
DisableMutationChance = 0.4
EnableMutationChance = 0.2



function pointMutate( genome )
    local step = genome.mutationRates[ "step" ]
   
    for i = 1, #genome.genes do
        local gene = genome.genes[ i ]
        if math.random() < PerturbChance then
            gene.weight = gene.weight + math.random() * step * 2 - step
        else
            gene.weight = math.random() * 4 - 2
        end
    end
end

function linkMutate(genome, forceBias)
        local neuron1 = randomNeuron(genome.genes, false)
        local neuron2 = randomNeuron(genome.genes, true)
         
        local newLink = newGene()
        if neuron1 <= Inputs and neuron2 <= Inputs then
                --Both input nodes
                return
        end
        if neuron2 <= Inputs then
                -- Swap output and input
                local temp = neuron1
                neuron1 = neuron2
                neuron2 = temp
        end
 
        newLink.into = neuron1
        newLink.out = neuron2
        if forceBias then
                newLink.into = Inputs
        end
       
        if containsLink(genome.genes, newLink) then
                return
        end
        newLink.innovation = newInnovation()
        newLink.weight = math.random()*4-2
       
        table.insert(genome.genes, newLink)
end
 
function nodeMutate(genome)
        if #genome.genes == 0 then
                return
        end
 
        genome.maxneuron = genome.maxneuron + 1
 
        local gene = genome.genes[math.random(1,#genome.genes)]
        if not gene.enabled then
                return
        end
        gene.enabled = false
       
        local gene1 = copyGene(gene)
        gene1.out = genome.maxneuron
        gene1.weight = 1.0
        gene1.innovation = newInnovation()
        gene1.enabled = true
        table.insert(genome.genes, gene1)
       
        local gene2 = copyGene(gene)
        gene2.into = genome.maxneuron
        gene2.innovation = newInnovation()
        gene2.enabled = true
        table.insert(genome.genes, gene2)
end
 
function enableDisableMutate(genome, enable)
        local candidates = {}
        for _,gene in pairs(genome.genes) do
                if gene.enabled == not enable then
                        table.insert(candidates, gene)
                end
        end
       
        if #candidates == 0 then
                return
        end
       
        local gene = candidates[math.random(1,#candidates)]
        gene.enabled = not gene.enabled
end

function mutate( genome )
    for mutation, rate in pairs( genome.mutationRates ) do
        if math.random( 1, 2 ) == 1 then
            genome.mutationRates[ mutation ] = 0.95 * rate
        else
            genome.mutationRates[ mutation ] = 1.05263 * rate
        end
    end

    if math.random() < genome.mutationRates[ "connections" ] then
        pointMutate( genome )
    end
   
    local p = genome.mutationRates["link"]
    while p > 0 do
            if math.random() < p then
                    linkMutate(genome, false)
            end
            p = p - 1
    end

    p = genome.mutationRates["bias"]
    while p > 0 do
            if math.random() < p then
                    linkMutate(genome, true)
            end
            p = p - 1
    end
   
    p = genome.mutationRates["node"]
    while p > 0 do
            if math.random() < p then
                    nodeMutate(genome)
            end
            p = p - 1
    end
   
    p = genome.mutationRates["enable"]
    while p > 0 do
            if math.random() < p then
                    enableDisableMutate(genome, true)
            end
            p = p - 1
    end

    p = genome.mutationRates["disable"]
    while p > 0 do
            if math.random() < p then
                    enableDisableMutate(genome, false)
            end
            p = p - 1
    end
end

function disjoint(genes1, genes2)
        local i1 = {}
        for i = 1,#genes1 do
                local gene = genes1[i]
                i1[gene.innovation] = true
        end
 
        local i2 = {}
        for i = 1,#genes2 do
                local gene = genes2[i]
                i2[gene.innovation] = true
        end
       
        local disjointGenes = 0
        for i = 1,#genes1 do
                local gene = genes1[i]
                if not i2[gene.innovation] then
                        disjointGenes = disjointGenes+1
                end
        end
       
        for i = 1,#genes2 do
                local gene = genes2[i]
                if not i1[gene.innovation] then
                        disjointGenes = disjointGenes+1
                end
        end
       
        local n = math.max(#genes1, #genes2)
       
        return disjointGenes / n
end

function breedChild(species)
        local child = {}
        if math.random() < CrossoverChance then
                g1 = species.genomes[math.random(1, #species.genomes)]
                g2 = species.genomes[math.random(1, #species.genomes)]
                child = crossover(g1, g2)
        else
                g = species.genomes[math.random(1, #species.genomes)]
                child = copyGenome(g)
        end
       
        mutate(child)
       
        return child
end
