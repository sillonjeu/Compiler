import re

class ModifiedSPLParser:
    def __init__(self, code):
        # 초기화시 코드를 토큰화하여 저장
        self.tokens = self.tokenize(code)
        self.current_token = self.tokens.pop(0)

    def tokenize(self, code):
        # 주어진 코드를 토큰화하여 리스트로 반환
        tokens = re.findall(r'[a-z]+|[0-9]+|=>|[+\-*/()]|<|>|=|,|:|\.|;|begin|end|label|integer|goto|if|then|else|fi|output|input', code)
        return [token for token in tokens if token.strip()]  # 빈 토큰 제거

    def consume(self, expected_token):
        # 예상 토큰과 현재 토큰을 비교하고 다음 토큰으로 이동
        if self.current_token == expected_token:
            self.current_token = self.tokens.pop(0) if self.tokens else None
        else:
            raise ValueError(f"Expected {expected_token}, but got {self.current_token}")

    def spl_program(self):
        # 전체 SPL 프로그램의 파싱 시작점
        self.block()
        self.consume('.')

    def block(self):
        # 'begin'과 'end' 사이의 코드 블록 파싱
        self.consume('begin')
        while self.current_token in ['label', 'integer']:
            self.dcl()
            self.consume(';')
        self.st_list()
        self.consume('end')

    def dcl(self):
        # 변수 선언 파싱
        self.consume(self.current_token)  # 'label' or 'integer'
        self.id()
        while self.current_token == ',':
            self.consume(',')
            self.id()

    def st_list(self):
        # 문장 리스트 파싱
        self.st()
        while self.current_token == ';':
            self.consume(';')
            self.st()

    def st(self):
        # 라벨이 있는지 확인 후, 해당 문장 파싱
        if self.current_token == 'label':
            self.consume('label')
            self.id()
            self.consume(':')
        self.statement()

    def statement(self):
        # SPL 내부의 각 문장에 따라 적절한 파싱 함수 호출
        if self.current_token == 'goto':
            self.goto_st()
        elif self.current_token == 'if':
            self.if_st()
        elif self.current_token == 'output':
            self.write_st()
        elif self.current_token == 'begin':
            self.block()
        else:
            self.assignment()

    def assignment(self):
        # 변수 할당 문장 파싱
        self.id()
        self.consume('=>')
        self.exp()

    def goto_st(self):
        # goto 문장 파싱
        self.consume('goto')
        self.id()

    def if_st(self):
        # 조건문 파싱
        self.consume('if')
        self.condition()
        self.consume('then')
        self.st_list()
        if self.current_token == 'else':
            self.consume('else')
            self.st_list()
        self.consume('fi')

    def write_st(self):
        # 출력 문장 파싱
        self.consume('output')
        self.consume('(')
        self.exp()
        while self.current_token == ',':
            self.consume(',')
            self.exp()
        self.consume(')')

    def condition(self):
        # 조건 파싱
        self.exp()
        if self.current_token in ['<', '>', '=']:
            self.consume(self.current_token)
        self.exp()

    def exp(self):
        # 표현식 파싱 (덧셈, 뺄셈)
        self.term()
        while self.current_token in ['+', '-']:
            self.consume(self.current_token)
            self.term()

    def term(self):
        # 항 파싱 (곱셈, 나눗셈)
        self.factor()
        while self.current_token in ['*', '/']:
            self.consume(self.current_token)
            self.factor()

    def factor(self):
        # 요소 파싱 (숫자, 식별자, 괄호 내 표현식)
        if self.current_token == 'input':
            self.consume('input')
        elif re.match(r'[a-z]+', self.current_token):
            self.id()
        elif re.match(r'[0-9]+', self.current_token):
            self.number()
        elif self.current_token == '(':
            self.consume('(')
            self.exp()
            self.consume(')')
        else:
            raise ValueError(f"Unexpected token {self.current_token} in factor")

    def id(self):
        # 식별자 파싱
        if re.match(r'[a-z]+', self.current_token):
            self.consume(self.current_token)
        else:
            raise ValueError(f"Expected an identifier, but got {self.current_token}")

    def number(self):
        # 숫자 파싱
        if re.match(r'[0-9]+', self.current_token):
            self.consume(self.current_token)
        else:
            raise ValueError(f"Expected a number, but got {self.current_token}")

    def parse(self):
        # 전체 프로그램 파싱 시작
        self.spl_program()
        if self.current_token is not None:
            raise ValueError(f"Unexpected token {self.current_token}")

# 수정된 SPL 코드를 사용하여 파서를 테스트
modified_spl_program_code = """
begin
    integer data, min, max, temp, diff, var;
    input => data;
    data => min;
    data => max;
    input => data;
    data => temp;
    temp => diff;
    if diff < 0 then
        min => temp;
    else
        temp => min;
    fi;
    diff => temp;       
    temp - 1 => var;
    var => temp;
    output(min, max);
end.

"""

parser = ModifiedSPLParser(modified_spl_program_code)
print(parser.tokens)
try:
    parser.parse()
    print("Parsing succeeded!")
except ValueError as e:
    print(f"Parsing failed: {e}")

# 실패이유 : 파서 구현 방식에서 연산 기능 부분을 제대로 구현 못한 것 같습니다..
