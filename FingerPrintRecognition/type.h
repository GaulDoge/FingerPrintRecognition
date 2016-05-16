#pragma once
//特征点类，用于表示指纹图像的特征点
class TraitPoint{
public:
	enum TP_T{ END, BUFI };
	TraitPoint() = default;
	TraitPoint(int x, int y, TP_T t, int d) {
		this->x = x;
		this->y = y;
		this->t = t;
		this->d = d;
	}

	int x;
	int y;
	TP_T t;
	int d;
};

//局部特征，用于表示指纹图像的局部特征
struct LocalTrait {
	int p[2];
	int t[2];
	int d[3];
	int r[3];
};