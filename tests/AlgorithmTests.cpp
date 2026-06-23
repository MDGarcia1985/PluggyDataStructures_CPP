/*
 * File: AlgorithmTests.cpp
 * Description: Tests pure algorithm modules separated from sessions, menus, and registries.
 * Copyright (c) 2026 Michael Garcia
 * Contact: michael@mandedesign.studio
 * Site: https://mandedesign.studio
 * SPDX-License-Identifier: MIT
 */

#include "TestHarness.h"

#include "algorithms/common/TargetOrdering.h"
#include "algorithms/graphs/GraphAnalysis.h"
#include "algorithms/graphs/GraphSearch.h"
#include "algorithms/graphs/ShortestPath.h"
#include "algorithms/graphs/GraphTraversal.h"
#include "algorithms/hashing/HashFunction.h"
#include "algorithms/hashing/LinearProbing.h"
#include "algorithms/hashing/SeparateChaining.h"
#include "algorithms/maps/FrequencyAnalysis.h"
#include "algorithms/maps/MapRanking.h"
#include "algorithms/maps/MapSearch.h"
#include "algorithms/sorting/InsertionSort.h"
#include "algorithms/sorting/QuickSort.h"
#include "algorithms/sorting/SelectionSort.h"
#include "algorithms/sorting/SortBenchmark.h"
#include "algorithms/trees/TreeAnalysis.h"
#include "algorithms/trees/TreeSearch.h"
#include "algorithms/trees/TreeTraversal.h"
#include "structures/TargetGraph.h"
#include "structures/TargetMap.h"
#include "structures/TargetTree.h"


using namespace pdstest;

namespace
{
    bool isSorted(const std::vector<pds::Target>& targets)
    {
        for (std::size_t index = 1; index < targets.size(); ++index)
        {
            if (pds::targetLess(targets[index], targets[index - 1]))
            {
                return false;
            }
        }
        return true;
    }

    pds::TargetGraph buildSampleGraph()
    {
        pds::TargetGraph graph;
        graph.addNode(pds::Target("A", "1"));
        graph.addNode(pds::Target("B", "2"));
        graph.addNode(pds::Target("C", "3"));
        graph.addEdgeByKey("A", "B", 1.0, false);
        graph.addEdgeByKey("B", "C", 1.0, false);
        return graph;
    }

    std::string keyForBucket(std::size_t bucket, std::size_t capacity, std::size_t ordinal)
    {
        std::size_t seen = 0;
        for (std::size_t candidate = 0; candidate < 100000; ++candidate)
        {
            const std::string key = "wrap" + std::to_string(candidate);
            if (pds::bucketIndexFor(key, capacity) == bucket)
            {
                if (seen == ordinal)
                {
                    return key;
                }
                ++seen;
            }
        }
        return "";
    }
}

PDS_TEST(testTargetOrderingTiers)
{
    const pds::Target alphaLower("alpha", "one");
    const pds::Target beta("beta", "one");
    const pds::Target alphaTwo("alpha", "two");

    expect(pds::targetLess(alphaLower, beta), "Primary field ordering is case-insensitive.");
    expect(pds::targetLess(alphaLower, alphaTwo), "Secondary field breaks ties.");
    expect(!pds::targetLess(alphaTwo, alphaLower), "Secondary field ordering is strict.");
    expect(pds::targetEqualsByOrdering(alphaLower, alphaLower), "Identical targets compare equal.");
}

PDS_TEST(testPureSortingAlgorithms)
{
    std::vector<pds::Target> empty;
    pds::insertionSort(empty);
    pds::selectionSort(empty);
    pds::quickSort(empty);
    expect(empty.empty(), "Empty vectors remain safe for every sort.");

    std::vector<pds::Target> single = {pds::Target("Only", "One")};
    pds::quickSort(single);
    expectEqual(single.size(), 1, "Single-item vectors remain safe for quick sort.");

    std::vector<pds::Target> targets = {
        pds::Target("Zulu", "2"),
        pds::Target("Alpha", "1"),
        pds::Target("Mike", "3"),
        pds::Target("alpha", "0")
    };

    pds::insertionSort(targets);
    expect(isSorted(targets), "Insertion sort orders by TargetOrdering.");

    targets = {
        pds::Target("Zulu", "2"),
        pds::Target("Alpha", "1"),
        pds::Target("Mike", "3"),
        pds::Target("alpha", "0")
    };
    pds::selectionSort(targets);
    expect(isSorted(targets), "Selection sort orders by TargetOrdering.");

    targets = {
        pds::Target("Zulu", "2"),
        pds::Target("Alpha", "1"),
        pds::Target("Mike", "3"),
        pds::Target("alpha", "0")
    };
    pds::quickSort(targets);
    expect(isSorted(targets), "Quick sort orders by TargetOrdering.");
}

PDS_TEST(testSortBenchmarkMetadata)
{
    const std::vector<pds::Target> baseline = {
        pds::Target("C", "3"),
        pds::Target("A", "1"),
        pds::Target("B", "2")
    };

    const pds::SortBenchmarkResult result = pds::benchmarkSort(
        "Insertion Sort",
        baseline,
        pds::insertionSort);

    expectEqual(result.algorithmName, "Insertion Sort", "Benchmark preserves the algorithm name.");
    expectEqual(result.itemCount, baseline.size(), "Benchmark reports the item count.");
    expect(result.repetitions > 0, "Benchmark chooses a positive repetition count.");
    expect(result.totalSeconds >= 0.0, "Benchmark reports total timing.");
    expect(result.averageSeconds >= 0.0, "Benchmark reports average timing.");
    expect(isSorted(result.sortedTargets), "Benchmark returns a sorted snapshot.");
}

PDS_TEST(testSeparateChainingStrategy)
{
    pds::SeparateChaining table(4);
    table.insert(pds::Target("Alpha", "1"));
    table.insert(pds::Target("Beta", "2"));
    table.insert(pds::Target("alpha", "updated"));

    pds::Target found;
    expect(table.lookup("Alpha", found), "Separate chaining finds stored keys.");
    expectEqual(found.fieldTwo(), "updated", "Separate chaining updates existing keys.");
    expectEqual(table.size(), 2, "Separate chaining keeps one entry per normalized key.");
    expect(table.loadFactor() > 0.0, "Separate chaining reports a positive load factor.");
    expect(table.bucketViews().size() >= 4, "Separate chaining exposes bucket views.");

    expect(table.remove("Beta"), "Separate chaining removes existing keys.");
    expect(!table.lookup("Beta", found), "Separate chaining reports removed keys as absent.");
}

PDS_TEST(testLinearProbingStrategy)
{
    pds::LinearProbing table(8);
    table.insert(pds::Target("One", "1"));
    table.insert(pds::Target("Two", "2"));
    table.insert(pds::Target("Three", "3"));

    pds::Target found;
    expect(table.lookup("Two", found), "Linear probing finds stored keys.");
    expect(table.probeCountFor("Two") >= 1, "Linear probing reports probe counts.");
    expect(table.longestCluster() >= 1, "Linear probing reports cluster length.");

    expect(table.remove("Two"), "Linear probing removes keys with tombstones.");
    expect(!table.lookup("Two", found), "Linear probing treats removed keys as absent.");
    expect(table.lookup("Three", found), "Linear probing still finds remaining keys after tombstones.");

    table.rehash(16);
    expectEqual(table.size(), 2, "Linear probing rehash preserves remaining entries.");
}

PDS_TEST(testLinearProbingWrappedClusterMetric)
{
    constexpr std::size_t capacity = 8;
    const std::string bucketSixA = keyForBucket(6, capacity, 0);
    const std::string bucketSeven = keyForBucket(7, capacity, 0);
    const std::string bucketSixB = keyForBucket(6, capacity, 1);

    expect(!bucketSixA.empty() && !bucketSeven.empty() && !bucketSixB.empty(), "Test keys are generated for wraparound probing.");

    pds::LinearProbing table(capacity);
    table.insert(pds::Target(bucketSixA, "first"));
    table.insert(pds::Target(bucketSeven, "second"));
    table.insert(pds::Target(bucketSixB, "third"));

    expectEqual(table.longestCluster(), static_cast<std::size_t>(3), "Linear probing treats end/start occupied slots as one circular cluster.");
}

PDS_TEST(testTreeAlgorithms)
{
    pds::TargetTree tree;
    tree.insert(pds::Target("M", "middle"));
    tree.insert(pds::Target("C", "left"));
    tree.insert(pds::Target("T", "right"));
    tree.insert(pds::Target("A", "leaf"));

    const std::vector<pds::Target> inOrder = pds::inOrder(tree);
    expectEqual(inOrder.size(), 4, "Tree traversal returns every node.");
    expect(isSorted(inOrder), "In-order traversal yields sorted output.");

    pds::Target found;
    expect(pds::find(tree, "C", found), "TreeSearch finds existing keys.");
    expect(pds::minimum(tree, found), "TreeSearch finds the minimum node.");
    expectEqual(found.fieldOne(), "A", "Minimum node uses ordering, not insertion order.");

    expectEqual(pds::height(tree), 3, "TreeAnalysis reports height.");
    expectEqual(pds::nodeCount(tree), 4, "TreeAnalysis reports node count.");
    expectEqual(pds::leafCount(tree), 2, "TreeAnalysis reports leaf count.");
    expect(pds::isBalanced(tree), "TreeAnalysis reports balance when subtree heights differ by at most one.");
}

PDS_TEST(testGraphAlgorithms)
{
    pds::TargetGraph graph = buildSampleGraph();

    const std::vector<pds::Target> bfs = pds::breadthFirst(graph, 0);
    expectEqual(bfs.size(), 3, "Graph BFS visits every reachable node.");
    expectEqual(bfs.front().fieldOne(), "A", "Graph BFS starts at the requested node.");

    const std::vector<pds::Target> dfs = pds::depthFirst(graph, 0);
    expectEqual(dfs.size(), 3, "Graph DFS visits every reachable node.");

    expect(pds::pathExists(graph, 0, 2), "GraphSearch finds reachable paths.");
    expect(pds::pathExists(graph, 2, 0), "GraphSearch follows undirected adjacency in reverse.");

    const std::vector<pds::Target> path = pds::shortestPathUnweighted(graph, 0, 2);
    expectEqual(path.size(), 3, "GraphSearch returns an unweighted shortest path.");
    expectEqual(path.front().fieldOne(), "A", "Shortest path starts at the source node.");

    const pds::ShortestPathResult shortest = pds::shortestPath(graph, 0, 2);
    expect(shortest.reachable, "Weighted shortest path reaches connected nodes.");
    expect(shortest.distance == 2.0, "Weighted shortest path sums positive edge weights.");

    expectEqual(pds::degree(graph, 1), 2, "GraphAnalysis reports node out-degree.");
    expectEqual(pds::connectedComponents(graph), 1, "GraphAnalysis counts connected components.");
    expect(!pds::hasCycle(graph), "GraphAnalysis does not treat one undirected edge pair as a cycle.");

    pds::TargetGraph triangle;
    triangle.addNode(pds::Target("A", "1"));
    triangle.addNode(pds::Target("B", "2"));
    triangle.addNode(pds::Target("C", "3"));
    triangle.addEdgeByKey("A", "B", 1.0, false);
    triangle.addEdgeByKey("B", "C", 1.0, false);
    triangle.addEdgeByKey("C", "A", 1.0, false);
    expect(pds::hasCycle(triangle), "GraphAnalysis detects an undirected triangle cycle.");

    pds::TargetGraph directed;
    directed.addNode(pds::Target("A", "1"));
    directed.addNode(pds::Target("B", "2"));
    directed.addNode(pds::Target("C", "3"));
    directed.addEdgeByKey("A", "B", 1.0, true);
    directed.addEdgeByKey("B", "C", 1.0, true);
    directed.addEdgeByKey("C", "A", 1.0, true);
    expect(directed.hasDirectedEdges(), "Graph records when directed edges are present.");
    expect(pds::hasCycle(directed), "GraphAnalysis detects directed cycles.");
    expect(!directed.addEdgeByKey("A", "C", -1.0, true), "Graph rejects negative edge weights for Dijkstra safety.");
}

PDS_TEST(testMapAlgorithms)
{
    pds::TargetMap map;
    map.countText("alpha beta alpha gamma beta delta");

    const auto top = pds::topN(map, 2);
    expectEqual(top.size(), 2, "FrequencyAnalysis returns top N entries.");
    expect(top[0].second >= top[1].second, "Top N is sorted by descending frequency.");

    const auto least = pds::leastFrequent(map, 1);
    expectEqual(least.size(), 1, "FrequencyAnalysis returns least frequent entries.");
    expect(least[0].second <= 2, "Least frequent entries use the smallest counts.");

    const auto buckets = pds::frequencyBuckets(map);
    expect(!buckets.empty(), "FrequencyAnalysis builds frequency buckets.");

    const auto byFrequency = pds::sortByFrequency(map);
    expectEqual(byFrequency.size(), map.size(), "MapRanking returns every entry by frequency.");
    expect(byFrequency.front().second >= byFrequency.back().second, "Frequency ranking is descending.");

    const auto alphabetical = pds::sortAlphabetically(map);
    expectEqual(alphabetical.size(), map.size(), "MapRanking returns every entry alphabetically.");
    expect(alphabetical.front().first <= alphabetical.back().first, "Alphabetical ranking is ascending.");

    expect(pds::containsWord(map, "Alpha"), "MapSearch finds normalized words.");
    expect(!pds::containsWord(map, "missing"), "MapSearch rejects absent words.");

    const std::vector<std::string> prefixMatches = pds::wordsStartingWith(map, "b");
    expect(!prefixMatches.empty(), "MapSearch finds prefix matches.");
    expect(prefixMatches.front().front() == 'b', "Prefix matches use normalized keys.");
}
