#!/bin/bash
size1="1"
size2="2"
size4="4"
size8="8"
kb="kb"
for i in {1..6};
do
	echo $size1
	fallocate -l $size1$kb data/$size1.in
	echo $size2
	fallocate -l $size2$kb data/$size2.in
	echo $size4
	fallocate -l $size4$kb data/$size4.in
	echo $size8
	fallocate -l $size8$kb data/$size8.in
	size1+="0"
	size2+="0"
	size4+="0"
	size8+="0"
done
