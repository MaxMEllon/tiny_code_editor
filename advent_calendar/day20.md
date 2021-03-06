本稿は[自作エディタをつくる Advent Calendar 2016](http://qiita.com/advent-calendar/2016/make_editor)の20日目です、レポジトリは[こちら](https://github.com/tinyco/tiny_code_editor)

# 自動テストの意義について思うこと

僕の主張としては、自動テストの有用性はわかるけど書くのとメンテが面倒くさいから、
バランスが大事だよねって思います。

形骸化してさして意味もないようなテストを数十分かけて実行して、
エラーが起きたらあまり考えずにテストケース側を直しながら、
いっぽうでテストを書いたから偉い！一箇所でも書かなかったらダメなやつだ！みたいなのもよくないし、

いっぽうで、今までテストを書いてない自分のコードも、
もしテストを書いていたらもう少しバグの発生を抑えれたはず？なので、
テストを全く書かないのも褒められたものではないですね。

なので、結局バランスの問題かなぁ、とか思ったり。
とりあえず、手軽な方法が好きな性格です。

# 自動テストのスタイルについて思うこと

11日目のコメントで、CUnitとj8takagiさん自身が作ったAutotest.mkをオススメされました。
ネットでみた感じだと、やはりCUnitと、ブランド力からかgoogle testが人気っぽい気がします。

もしIDEを使っているともう少し選択肢があるのですが、
エディタを作っているので選択肢には入れません！！

ざっくり言うと、CUnitとgoogle testはよくあるアサーションのやつで、

```
assert_equal(1, 1);//ok
assert_equal(1, 2);//ng
```

こういうやつです。
shouldって単語を使う派閥等もありますが、C言語は全体的にassert派らしい。

それに対して、Autotest.mkは
とりあえずprintfしていって、正解のテキストと一致したらOK、
というあまり見ないスタイルのフレームワークです。

より正確に言うと、与えられたコマンドを全て実行して、
標準出力の結果をテストケースとして書かれたテキストファイルと比べる。
一致していたらテストが通ったとみなす。という感じです。

このスタイルは明らかにC言語でテキストエディタを作るときに便利で、
さらに他の言語・他の用途でも結構便利な気がします。

メリットの１つめは一気にアサーションできる点です。
例えば、複雑なプロパティを持つオブジェクトを、
`console.log(JSON.stringify(target,null,2))`
とかしたら１行で全部チェックできるし、

今回のエディタだと

```
while (current_lines) {
  printf("%s, ", current_lines->mutable_string);
  current_lines = current_lines->next;
}
```

みたいな感じでリスト構造を一気にチェックできます。

同じテストは、よくあるassertのテストフレームワークだと

```
assert_equal("abc", current_lines->mutable_string);
current_lines = current_lines->next;
assert_equal("def", current_lines->mutable_string);
current_lines = current_lines->next;
assert_equal("ghi", current_lines->mutable_string);
```

みたいに、ちまちまチェックしていかないといけないので面倒そうですね。


あと、エディタを作るのに限った話として、作るものにprintfっぽいものが多いので
そのままテストできると楽です。

メリットの２つめは、標準出力をテスト結果とするため、コマンドを実行できる点です。
./test_case_1がファイルa.txtに文章を保存するときは

```
./test_case_1
cat a.txt
```

みたいな感じでファイルにちゃんと保存できたか確認できる点が良いですね。
他の言語でも

```
swift ./file_upload.swift hello.txt
curl http://target.com/hello.txt
```

みたいに開発サーバと連携する部分とかも試したりできそうです。

よく、「テストしやすいように副作用がない関数にしなさい」と言う人がいますが、
`file_upload.swift` こいつはファイル名からして副作用がありそうです。
そういうときもなんとかできる感がよいと思いました。
テストケース内でexecすればいい気もするけど、手軽にコマンド使えるのはやはり便利な気がしますね。

メリットの３つめは、開発言語から切り離しやすい点ですね。
複数の言語を跨いで何かを作ってる状況で便利そうです。自分の職場だとたまにあります。
この連載みたいに、初めての言語に挑む時もテストは慣れた手段でできたら嬉しい気がしますね。

他に、作者のページから引用すると

> 使い方がシンプル
> さまざまなプログラムや環境に対応
> ビルド作業と連携しやすい
> 無償で使うことができ、自由に改造できる

がオススメポイントのようです。

逆にデメリットに感じた部分は、makefileはあまり馴染みがないかな、という点です。
マニュアルをよく読むと、そういう人でも扱いやすくする工夫が垣間見えるのですが、
やはり、とっつきにくい印象がある気がしました。
自分はあまりmakefileをマスターできてないですが、慣れの問題でしょうか。

具体的には、テストケースを増やすときに、コマンドラインで動かすための部分

```
#include "sample.h"

int main(int argc, char *argv[]) {
  if (strcmp(argv[1],"test1") == 0) {
    test1();
  } else if (strcmp(argv[1],"test2") == 0) {
    test2();
  } ...
}
```

たぶんこういうのがテストケースを入れるフォルダごとに要るのかな、と思って
実行ファイルを作るmakefileの量が増えるのが気になりました。
ただ、CUnitもたいがい準備が面倒なので、C言語だと手間は変わらない気がします。
補足すると、テスト結果の判定に使うテキストファイルはautotest.mkの便利コマンドでサクッと作れそうです。

結論として、autotest.mkが一番便利そうだと思いました。
発想自体が良いと思うので、自作で真似してみるのも楽しそうです。
ちょっとやってみようかな・・・

なんにせよ、テスト実行用のmain関数を別途用意する必要がありそうですね。

# テスト用のメイン関数をつくる

試しに、上記のようなtest.cを作ってみます。makefileもそれっぽく書き足します。
すると実行ファイルとともに、main.dSYM / test.dSYMファイルが出てきました。・・・なぜ？

ググるとiOSの記事ばかりですが、どうやらクラッシュ時にメモリアドレスと関数名を紐づけるためのファイルらしいです。
makefileをいじった感じだと、複数の実行ファイルを`-g`オプション付きでコンパイルすると生成されるようです。
あとで使うかもしれないし、とりあえずgitignoreしておきます。

とりあえずソースコード1ファイル分のテストケースを作ってみました。

```
$ ./test utf8char
test - utf8char
- char あ
- size 3
- is_\n 0
- width 2
finish
```

意味は「あ」は3バイト、改行ではない意味の0、全角の2だよ、です。
ここに差分が出るとデグレですね。

# テストフレームワークを使わないテスト

なんか作ってるうちに気づいたのですが、
この結果をgitに入れてしまえば苦労せずにdiffとれるのでは。
差が出てたらコミットする前に気づくし、コミット後に気づくCI環境より便利かもしれない。

作ったシェルスクリプト

```
./test utf8char > test_result/utf8char.txt
./test mutable_string > test_result/mutable_string.txt
```

このシェルスクリプトをgulpとmakefileに登録しておきます。
（test.cって名前にすると、`make test`が使えないんですね）

なんか身も蓋もない感じだけど、別にgitを使うこの方法でも困らないよなって気がしてきました。
かなり手軽に始められるし、何かの学び始めはこれでいけばいい気がしますね。
プログラミング言語関係ないし、もしDBあると勝手が違うかもしれないけど、コマンド叩けるのでクリーンアップも余裕では・・・。

ちなみにシェルスクリプトなので、`&`とかつけると並列実行もお手軽です。

git、こんな使い方があったのか。

# 今日のまとめ

- 自動テストを始めた、お手軽だったので後回しにしなければよかった
  - なんか難しいことを考えたわりに、ライフハックてきな小技に落ち着いた

コメントで教えてもらったvalgrindは便利そうなんですが、
MacOS Sierraだとうまく動かせず(brewにいない、valgrindのmakeが通らない)

Autotest.mkを使うとvalgrindと連携できたり、かかった時間を計測したりできるので上記の小技より高機能です。
valgrind動かせたらもう一度導入を検討しようと思います。
そのころにはテストケースも充実してファイルを分割したくなっている・・・はず。

どちらの方法にせよtest.cは共通でいけるし、ちょっとgitで様子見かな。
