.PHONY: reconf
reconf:
	meson setup --reconfigure build

.PHONY: compile
compile:
	meson compile -C build

.PHONY: test
test:
	SPDLOG_LEVEL=debug meson test -C build -v

.PHONY: clean
clean:
	rm -rf build
	make reconf

.PHONY: pytest
pytest:
	# pytest -s ./tests/test_buddy_allocator.py
	# pytest -s ./tests/test_binary_search_tree.py
	pytest -s ./tests/test_btree.py