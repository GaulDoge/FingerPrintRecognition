#pragma once
//�������࣬���ڱ�ʾָ��ͼ���������
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

//�ֲ����������ڱ�ʾָ��ͼ��ľֲ�����
struct LocalTrait {
	int p[2];
	int t[2];
	int d[3];
	int r[3];
};