module Derp where
import Data.List

toTernary :: Int -> Int -> [Int]
toTernary p 0 = take p $ repeat 0
toTernary p n
    | r == 0 = rest ++ [0]
    | r == 1 = rest ++ [1]
    | r == 2 = rest ++ [2]
    where
        r = mod n 3
        m = div n 3
        rest = toTernary (p-1) m

type Tri = (Int,Int,Int)
type Adj = (Int,Int)

-- Vertices that make up each triangle in the cube
cubeVerts :: [Tri]
cubeVerts = [
        (1,2,3), (1,0,2),
        (3,7,5), (3,5,1),
        (7,6,4), (7,4,5),
        (6,2,0), (6,0,4),
        (2,6,3), (3,6,7),
        (4,0,1), (4,1,5)
    ]

-- Adjacencies between triangle faces. We need to make sure
-- that no two adjacent triangles (faces) have the same color.
triAdj :: [Adj]
triAdj = [
        (0,1), (0,3), (0,8),
        (1,6), (1,10),
        (2,3), (2,5), (2,9),
        (3,11),
        (4,5), (4,7), (4,9),
        (5,11),
        (6,7), (6,8),
        (7,10),
        (8,9),
        (10,11)
    ]

-- Rotates the vertices in a way that preserves the order 
-- but gives a different provoking vertex for the triangle
rotateTri :: Tri -> Int -> Tri
rotateTri tri@(a,b,c) n
    | i == 0 = tri
    | i == 1 = (c,a,b)
    | i == 2 = (b,c,a)
    where
        i = mod n 3

rotate :: [Tri] -> [Int] -> [Tri]
rotate tris amts = zipWith (\tri amt -> rotateTri tri amt) tris amts

-- Test whether the provoking vertices of the members of the
-- given adjacency are the same
isAdjacent :: [Tri] -> Adj -> Bool
isAdjacent tris (a,b) = a' == b'
    where
        (a',_,_) = (!!) tris a
        (b',_,_) = (!!) tris b

-- Test whether the given the given configuration is valid
-- (i.e. no adjacent triangles with the same provoking vertex)
isGoodConfig :: [Tri] -> [Adj] -> Bool
isGoodConfig tris adjs = not $ any (isAdjacent tris) adjs

countColors :: [Tri] -> Int
countColors tris = length $ nub $ map (\(a, b, c) -> a) tris

main = do {
    let n = length cubeVerts in
        maximumBy (\a -> \b -> compare (countColors a) (countColors b)) $ 
            filter (\x -> isGoodConfig x triAdj) $ 
            map (\x -> (rotate cubeVerts (toTernary n x))) [0..3^n]
    --putStrLn $ show $ isGoodConfig cubeVerts triAdj
}