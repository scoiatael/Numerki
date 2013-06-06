DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && cd ..  && pwd )"
cd $DIR/doc
pdflatex sprawozdanie.tex
pdflatex sprawozdanie.tex
for var in out log aux
do
	rm "sprawozdanie."$var
done
