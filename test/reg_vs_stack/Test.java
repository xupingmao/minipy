public class Test {

	public static final int N = 1000000;
	public static final int RADD = 0;
	public static final int RRET = 1;
	public static final int LDL = 2;
	public static final int ADD = 3;
	public static final int STL = 4;
	public static final int RET = 5;
	
	static class RegInst {
		public int op;
		public int a;
		public int b;
		public int c;
		
		public RegInst(int op, int a, int b, int c) {
			this.op = op;
			this.a = a;
			this.b = b;
			this.c = c;
		}
	}
	
	static class StackInst {
		public int op;
		public int a;
		
		public StackInst(int op, int r) {
			this.op = op;
			a = r;
		}
		public StackInst(int op) {
			this.op = op;
		}
	}
	
	public static int execute(RegInst instList[]) {
		int locals[] = new int[] {1,2,3,4,5,6,7,8,9};
		for (int i = 0; i < N; i++) {
			for (RegInst inst: instList) {
				switch(inst.op) {
				case RADD:
					locals[inst.a] = locals[inst.b]+locals[inst.b]; 
					break;
				case RRET:
					return locals[inst.a];
				}
			}
		}
		return 0;
	}
	
	public static int execute(StackInst instList[]) {
		int locals[] = new int[] {1,2,3,4,5,6,7,8,9};
		int stack[] = new int[10];
		for (int i = 0; i< N; i++) {
			int top = 0;
			for (StackInst inst: instList) {
				switch(inst.op) {
				case ADD:
					int a = stack[top];
					top--;
					int b = stack[top];
					stack[top] = a+b;
					break;
				case LDL:
					top++;
					stack[top] = locals[inst.a];
					break;
				case STL:
					top--;
					locals[inst.a] = stack[top];
					break;
				case RET:
					return stack[top];
				}
			}
		}
		return 0;
	}
	
	public static void main(String[] args) {
		RegInst regInst = new RegInst(RADD, 1, 2, 3);
		long t1, t2;
		t1 = System.currentTimeMillis();
		execute(new RegInst[]{regInst});
		t2 = System.currentTimeMillis();
		System.out.println("reg=" + (t2-t1));
		
		StackInst s01 = new StackInst(LDL, 2);
		StackInst s02 = new StackInst(LDL, 3);
		StackInst s03 = new StackInst(ADD);
		StackInst s04 = new StackInst(STL, 1);
		
		t1 = System.currentTimeMillis();
		execute(new StackInst[] {s01, s02, s03, s04});
		t2 = System.currentTimeMillis();
		System.out.println("stack=" + (t2-t1));
	}
}
