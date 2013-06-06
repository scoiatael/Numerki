import Data.Monoid
import Data.List (tails)

data Matrix = M Int Int Int Int deriving Show

mult_m :: Matrix -> Matrix -> Matrix

(M a11 a12 a21 a22) `mult_m` (M b11 b12 b21 b22) = M (a11*b11 + a21*b12) (a12*b11 + a22*b12) (a11*b21 + a21*b22) (a12*b21 + a22*b22) 

instance Monoid Matrix where
	mappend = mult_m
	mempty = M 1 0 0 1

pot n = let list1110 = (M 1 1 1 0) : list1110 in map (foldr mult_m mempty) $ map (\x -> take x list1110) [1..n]
