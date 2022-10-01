if __name__ == "__main__":
    total = ''
    with open('changelog.md', encoding='utf-8') as f:
        total = f.readlines()
    content = ''
    index = 0
    for x in total:
        x = x.strip()
        if len(x) != 0:
            tokens = x.split(' ')
            if tokens[0] == '###':
                index += 1
            if index == 2:
                break
            content += x + '\n'
    with open('release_note', "w", encoding="utf-8") as f:
        f.write(content)
