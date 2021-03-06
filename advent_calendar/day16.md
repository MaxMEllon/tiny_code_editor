本稿は[自作エディタをつくる Advent Calendar 2016](http://qiita.com/advent-calendar/2016/make_editor)の16日目です、レポジトリは[こちら](https://github.com/tinyco/tiny_code_editor)

# バグ

昨日分を公開した直後に、カーソルがたくさん出てしまうバグをこっそりなおしました。
めっちゃケアレスミスでした

今度こそ、この文章を作ったエディタで書いて・・・って思ったけど、冒頭の定型文を前日文からコピペすると
`main(9961,0x7fffbcfd63c0) malloc: *** error for object 0x7f9fb6c02768: incorrect checksum for freed object - object was probably modified after being freed.`
が出てきたので、結局VS Codeで書いてます。直さなきゃ！
(いつも作業メモをとりながらコードを書いて、それを手直しして公開しています)

# プルリクエスト

ありがたいことに、akinomyoga氏から2件プルリクエストをもらったので、読んでいきます。

## プルリクエスト1件目 makefile

頂いたプルリクエストから引用すると

> Makefile の修正
> ・ .o: → .c.o:
> ・ .c.o ルールに入力ファイル $< が指定されていなかったのを修正
> ・ リンク時に指定している -Wall -Wextra オプションをコンパイル時にも指定

diffをみると

```
-.o:
-gcc -g -c $@
+.c.o:
+gcc -g -Wall -Wextra -c -o $@ $<
```

順番が前後しますが

・`指摘3件目`のオプションの件はdiffをみてすぐに理解できました。いままでxxx.c->xxx.oのときは-Wall -Wextraをつけておらず、.oの結合のときにつけてました。両方つけないとダメだよ、という旨です。
8日目にこのオプションをつけた時に

> 今まで無視されていた警告もでるようですが、現時点では効果が実感できません。

と書いてましたが、当たり前ですね・・・。警告が本来は出ていましたが、スルーしてしまっていました。



・`指摘2件目` の`$<`は、どのヘッダファイルを読んでコンパイルするかの話で、一般論として`$<`をつけるべきのようです。
自分のレポジトリのヘッダファイルはオレオレ仕様で、[これ](https://github.com/tinyco/clang_gen_header)を使って自動生成しているので、インクルードガードが全部ついています。
そこに甘えて、困ったら適当にincludeを足しているのでビルド自体は通るのですが、本来なら不要なincludeがあることを示唆していそうです。
あとで不要なincludeを削っていくようにしたいです。

コメントで教えてもらった内容によると、`$^`を指定する場合もあるが、`BSD make`のみ対応のようです。
両者で指定範囲が異なるものの、僕の理解だと依存先の依存先の・・・というように末尾に達してからコンパイルを始めるために、結果としては変わらない・・のかな？(自信がない)


・`指摘1件目` の拡張子を`.c.o`にするのは、慣習的にはそうするよ、という感じでしょうか？
世の中のMakefileをgithubで眺めてみます。他の人のMakefileめっちゃ長いな・・・
他の人のMakefileだと、`%.o`になっているのが多いのですが、自分のをそうするとビルドが通らなくなります。
最終的に、拡張子が.oになるのは正しいが、指定は`*.o`にするべきなのではないか？という気がしてきました。

そのあたりで悩んでいると、issueにコメントをいただきます。そのコメントを頼りに、勘違いがいくつか明らかになります

- `.c.o`は拡張子ではなくサフィックスルールと呼び、拡張子が`.o`になるものにマッチする
- `.o`または`*.o`は指定として正しくない。正しくないものを指定すると、`gcc -g -c $@`には来ない。
  - 結果的にデフォルト設定にフォールバックして、`gcc`ではなく`cc`でビルドが通る

また、ccにフォールバックしなくなったので`-g`オプションが効いて、lldbの出力が少し賢くなった気がします。
まだデバッガを使いこなせてないですが「よくわからないけど思い通り動かない」状態を脱したので、慣れていきたいところ。

## プルリクエスト2件目　セグフォ

> 試してみたのですけれど行末で改行すると Segmentation fault しました。
> 取り敢えず修正しました。

申し訳なさがすごいです、冒頭のエラーはそのせいでしょうか。
自分でも再現するか確認して、普通に100%再現しました。言い訳の余地がない・・・
メッセージが`Segmentation fault: 11`なので、冒頭のエラーではなさそうですね。

とりあえず、もらったプルリクエストで動作を確認してみます。
指摘された問題はなおっていますが、ファイルの末尾で改行した後にバックスペースを押すと同様に`Segmentation fault: 11`で落ちます。
どんどん出てくるバグ・・・

コミットを分けてくれているので、順に読んでいきます。

- null-checkの考慮漏れ
  - `if (current->next)` でnull-checkしていたが、`if (current)` と `if (current->next)`で2回チェックしないといけない。
  - たぶん、似たミスをしているところが他にもある
- スペルミス devide -> divide
- null-checkの考慮漏れ、場所は違うが理由は同じ

インデント、空白とタブが混ざっていてた・・・、あとで直さなきゃ

# バグ修正・・・？

上で見つかったバグたちを、自作エディタを使って順に潰していきます。

- 落ちる
- 保存するファイル名の最後が化ける(フォルダ+ファイル名指定時)
- 明らかに機能が少ない

ので、結構辛いです。現時点だと（C言語だと）、行数表示が欲しい。
他の言語で欲しいと思ったことはあんまりなかったですが。

しかし、動作確認しながら該当コードも読めるっていう一石二鳥な体験？ができます！
ただ、ビルドが通らなくなると(gulpで自動的にmake cleanされて)エディタがなくなるので
予備のエディタとgitが基本的に生命線です。

とりあえず、gccのwarningを直して、(15日目コメントより)保存をfwriteにしてみました。

ちょっと心がくじけてきたので！？、続きは明日にします。

# インデントのタブを空白に

わりとタブで揃ってたのですが、

```
$ find . | grep '\.c$' | sed 's/\(.*\)/ expand -t 2 \1 > \1.bak /g' | sh
$ find . | grep '\.c\.bak$' | sed 's/\(.*\)\.bak/ mv \1.bak \1 /g' | sh
```

でまとめて空白に直しました。

タブ文字、僕の環境だとコンソール上の見た目の文字幅が半角ではない・・・つらい(半角扱いされてる)

# 今日のまとめ

- どんどん出てくるバグ
- さらにどんどん出てくるバグ
- 人生初の、会社ではない人からのプルリクエスト
- マリオラン1-3クリア

プルリクエストほんとうにありがとうございました。
curses、調べよう調べようと思ってやってないので明日こそ調べるぞ！